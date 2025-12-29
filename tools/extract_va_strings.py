import argparse
import json
import os
import struct
from dataclasses import dataclass
from typing import Iterable, Optional


def read_u16le(data: bytes, offset: int) -> int:
    return struct.unpack_from("<H", data, offset)[0]


def read_u32le(data: bytes, offset: int) -> int:
    return struct.unpack_from("<I", data, offset)[0]


@dataclass(frozen=True)
class Section:
    name: str
    virtual_address: int
    virtual_size: int
    raw_ptr: int
    raw_size: int

    def contains_rva(self, rva: int) -> bool:
        size = max(self.virtual_size, self.raw_size)
        return self.virtual_address <= rva < (self.virtual_address + size)

    def rva_to_file_offset(self, rva: int) -> int:
        return self.raw_ptr + (rva - self.virtual_address)


@dataclass(frozen=True)
class PEInfo:
    image_base: int
    sections: list[Section]


def parse_pe(path: str) -> PEInfo:
    with open(path, "rb") as f:
        data = f.read()

    if data[:2] != b"MZ":
        raise ValueError("Not an MZ executable")

    e_lfanew = read_u32le(data, 0x3C)
    if data[e_lfanew : e_lfanew + 4] != b"PE\x00\x00":
        raise ValueError("Invalid PE signature")

    file_header_off = e_lfanew + 4
    number_of_sections = read_u16le(data, file_header_off + 2)
    size_of_optional_header = read_u16le(data, file_header_off + 16)

    optional_header_off = file_header_off + 20
    magic = read_u16le(data, optional_header_off)
    if magic not in (0x10B, 0x20B):
        raise ValueError(f"Unsupported optional header magic: 0x{magic:04x}")

    # PE32: ImageBase at +0x1C; PE32+: at +0x18
    image_base_off = optional_header_off + (0x1C if magic == 0x10B else 0x18)
    image_base = read_u32le(data, image_base_off) if magic == 0x10B else struct.unpack_from("<Q", data, image_base_off)[0]

    section_table_off = optional_header_off + size_of_optional_header
    sections: list[Section] = []
    for i in range(number_of_sections):
        off = section_table_off + i * 40
        name = data[off : off + 8].split(b"\x00", 1)[0].decode(errors="replace")
        virtual_size = read_u32le(data, off + 8)
        virtual_address = read_u32le(data, off + 12)
        raw_size = read_u32le(data, off + 16)
        raw_ptr = read_u32le(data, off + 20)
        sections.append(
            Section(
                name=name,
                virtual_address=virtual_address,
                virtual_size=virtual_size,
                raw_ptr=raw_ptr,
                raw_size=raw_size,
            )
        )

    return PEInfo(image_base=int(image_base), sections=sections)


def va_to_file_offset(pe: PEInfo, va: int) -> int:
    if va < pe.image_base:
        raise ValueError(f"VA 0x{va:08x} < ImageBase 0x{pe.image_base:08x}")

    rva = va - pe.image_base
    for section in pe.sections:
        if section.contains_rva(rva):
            return section.rva_to_file_offset(rva)
    raise ValueError(f"RVA 0x{rva:08x} not in any section")


def read_c_string(data: bytes, start: int, max_len: int) -> str:
    end = start
    limit = min(len(data), start + max_len)
    while end < limit and data[end] != 0:
        end += 1
    raw = data[start:end]

    # Try ASCII first; fall back to latin1 for 0x80-0xFF bytes.
    try:
        return raw.decode("ascii")
    except UnicodeDecodeError:
        return raw.decode("latin1")


def read_w_string(data: bytes, start: int, max_chars: int) -> Optional[str]:
    # Heuristic: UTF-16LE string (every second byte 0) until double-null.
    chars: list[int] = []
    off = start
    for _ in range(max_chars):
        if off + 2 > len(data):
            return None
        ch = read_u16le(data, off)
        if ch == 0:
            break
        chars.append(ch)
        off += 2
    if not chars:
        return ""
    try:
        return bytes(struct.pack("<" + "H" * len(chars), *chars)).decode("utf-16le")
    except UnicodeDecodeError:
        return None


def parse_int_auto(value: str) -> int:
    value = value.strip().lower()
    return int(value, 16) if value.startswith("0x") else int(value, 10)


def main() -> int:
    parser = argparse.ArgumentParser(description="Extract C/UTF-16LE strings from PE by VA")
    parser.add_argument("--exe", required=True, help="Path to PE executable")
    parser.add_argument(
        "--va",
        required=True,
        action="append",
        help="Virtual address (hex like 0x48CF80 or decimal)",
    )
    parser.add_argument("--max-len", type=int, default=1024, help="Max bytes/chars to read")
    parser.add_argument("--json", action="store_true", help="Output JSON")
    args = parser.parse_args()

    exe_path = args.exe
    if not os.path.exists(exe_path):
        raise FileNotFoundError(exe_path)

    pe = parse_pe(exe_path)
    with open(exe_path, "rb") as f:
        blob = f.read()

    results = []
    for va_str in args.va:
        va = parse_int_auto(va_str)
        file_off = va_to_file_offset(pe, va)

        ascii_s = read_c_string(blob, file_off, args.max_len)
        # Try UTF-16LE only if looks plausible (first byte printable and second byte 0)
        w_s = None
        if file_off + 1 < len(blob) and blob[file_off + 1] == 0:
            w_s = read_w_string(blob, file_off, args.max_len)

        results.append(
            {
                "va": f"0x{va:08x}",
                "file_offset": f"0x{file_off:08x}",
                "ascii": ascii_s,
                "utf16le": w_s,
            }
        )

    if args.json:
        print(json.dumps({"image_base": f"0x{pe.image_base:08x}", "results": results}, ensure_ascii=False, indent=2))
    else:
        print(f"ImageBase: 0x{pe.image_base:08x}")
        for r in results:
            print(f"VA {r['va']} -> file {r['file_offset']}")
            print(f"  ascii:   {r['ascii']!r}")
            print(f"  utf16le: {r['utf16le']!r}")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
