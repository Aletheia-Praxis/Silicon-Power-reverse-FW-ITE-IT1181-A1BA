import argparse
import struct
from dataclasses import dataclass
from pathlib import Path


@dataclass(frozen=True)
class PeSection:
  name: str
  virtual_address: int
  virtual_size: int
  raw_ptr: int
  raw_size: int


def _u16(data: bytes, off: int) -> int:
  return struct.unpack_from('<H', data, off)[0]


def _u32(data: bytes, off: int) -> int:
  return struct.unpack_from('<I', data, off)[0]


def _parse_sections(data: bytes) -> tuple[int, list[PeSection]]:
  if data[:2] != b'MZ':
    raise ValueError('Not an MZ executable')

  e_lfanew = _u32(data, 0x3C)
  if data[e_lfanew:e_lfanew + 4] != b'PE\x00\x00':
    raise ValueError('Not a PE executable')

  file_hdr_off = e_lfanew + 4
  num_sections = _u16(data, file_hdr_off + 2)
  size_opt = _u16(data, file_hdr_off + 16)

  opt_off = file_hdr_off + 20
  magic = _u16(data, opt_off)
  if magic != 0x10B:
    raise ValueError(f'Expected PE32 (0x10B), got {magic:#x}')

  image_base = _u32(data, opt_off + 28)
  sec_off = opt_off + size_opt

  sections: list[PeSection] = []
  for i in range(num_sections):
    off = sec_off + i * 40
    name = data[off:off + 8].split(b'\x00', 1)[0].decode('ascii', 'ignore')
    virtual_size = _u32(data, off + 8)
    virtual_address = _u32(data, off + 12)
    raw_size = _u32(data, off + 16)
    raw_ptr = _u32(data, off + 20)
    sections.append(
      PeSection(
        name=name,
        virtual_address=virtual_address,
        virtual_size=virtual_size,
        raw_ptr=raw_ptr,
        raw_size=raw_size,
      )
    )

  return image_base, sections


def _rva_to_file_off(sections: list[PeSection], rva: int) -> int:
  for sec in sections:
    sec_span = max(sec.virtual_size, sec.raw_size)
    if sec.virtual_address <= rva < sec.virtual_address + sec_span:
      return sec.raw_ptr + (rva - sec.virtual_address)
  raise KeyError(f'RVA not mapped to file: {rva:#x}')


def _format_ranges(values: list[int]) -> str:
  if not values:
    return ''

  ranges: list[str] = []
  start = prev = values[0]
  for v in values[1:]:
    if v == prev + 1:
      prev = v
      continue
    if start == prev:
      ranges.append(str(start))
    else:
      ranges.append(f'{start}-{prev}')
    start = prev = v

  if start == prev:
    ranges.append(str(start))
  else:
    ranges.append(f'{start}-{prev}')

  return ','.join(ranges)


def main() -> int:
  parser = argparse.ArgumentParser()
  parser.add_argument('--exe', required=True, help='Path to original URescue_v81D.2.24.2.exe')
  parser.add_argument('--map-addr', default='0x0040D610', help='Absolute address of byte mapping table')
  parser.add_argument('--jt-addr', default='0x0040D5FC', help='Absolute address of jump table (DWORD addresses)')
  parser.add_argument('--max-code', default='0x74', help='Max BCM error code handled by the table (inclusive)')
  args = parser.parse_args()

  exe_path = Path(args.exe)
  data = exe_path.read_bytes()

  image_base, sections = _parse_sections(data)

  map_addr = int(args.map_addr, 16)
  jt_addr = int(args.jt_addr, 16)
  max_code = int(args.max_code, 16)

  rva_map = map_addr - image_base
  rva_jt = jt_addr - image_base

  map_off = _rva_to_file_off(sections, rva_map)
  jt_off = _rva_to_file_off(sections, rva_jt)

  map_len = max_code + 1
  mapping = list(data[map_off:map_off + map_len])
  if len(mapping) != map_len:
    raise ValueError('Failed to read mapping table')

  max_index = max(mapping)
  jump_table = [
    _u32(data, jt_off + i * 4)
    for i in range(int(max_index) + 1)
  ]

  print(f'image_base={image_base:#x}')
  print(f'map_addr={map_addr:#x} rva={rva_map:#x} file_off={map_off:#x} len={map_len}')
  print(f'jt_addr={jt_addr:#x} rva={rva_jt:#x} file_off={jt_off:#x} entries={len(jump_table)}')
  print('jump_table:')
  for i, addr in enumerate(jump_table):
    print(f'  idx={i} addr={addr:#010x}')

  print('mapping_by_index:')
  for idx in range(int(max_index) + 1):
    codes = [code for code, mapped_idx in enumerate(mapping) if mapped_idx == idx]
    print(f'  idx={idx} codes={_format_ranges(codes)}')

  print('mapping_bytes_hex:')
  print(' '.join(f'{b:02x}' for b in mapping))

  return 0


if __name__ == '__main__':
  raise SystemExit(main())
