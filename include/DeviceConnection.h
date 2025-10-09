/**
 * DeviceConnection.h - Заголовок системи підключення до пристрою
 *
 * Визначає функції для управління підключенням до USB пристроїв ITE IT1181-A1BA
 */

#pragma once

#include <afx.h>

#include "WindowsHeaders.h"


// Forward declarations
class CUrescueDlg;
class iTEUFDrs;

/**
 * DeviceConnectionManager - управління підключенням до пристрою
 */
namespace DeviceConnectionManager {

/**
 * Підключитися до пристрою
 * @param pDialog Вказівник на головний діалог для оновлення UI
 * @param pDeviceManager Вказівник на менеджер пристроїв iTEUFDrs
 * @return TRUE якщо підключення успішне, FALSE інакше
 */
BOOL ConnectToDevice(CUrescueDlg* pDialog, iTEUFDrs* pDeviceManager);

/**
 * Відключитися від пристрою
 * @param pDialog Вказівник на головний діалог для оновлення UI
 * @param pDeviceManager Вказівник на менеджер пристроїв iTEUFDrs
 * @return TRUE якщо відключення успішне, FALSE інакше
 */
BOOL DisconnectFromDevice(CUrescueDlg* pDialog, iTEUFDrs* pDeviceManager);

/**
 * Перевірити стан підключення
 * @param pDeviceManager Вказівник на менеджер пристроїв iTEUFDrs
 * @return TRUE якщо пристрій підключено, FALSE інакше
 */
BOOL IsDeviceConnected(iTEUFDrs* pDeviceManager);

/**
 * Отримати інформацію про підключений пристрій
 * @param pDeviceManager Вказівник на менеджер пристроїв iTEUFDrs
 * @return Рядок з інформацією про пристрій
 */
CString GetDeviceConnectionInfo(iTEUFDrs* pDeviceManager);

/**
 * Автоматичне підключення при старті
 * @param pDialog Вказівник на головний діалог для оновлення UI
 * @param pDeviceManager Вказівник на менеджер пристроїв iTEUFDrs
 * @return TRUE якщо автопідключення успішне, FALSE інакше
 */
BOOL AutoConnectDevice(CUrescueDlg* pDialog, iTEUFDrs* pDeviceManager);

}  // namespace DeviceConnectionManager
