/**
 * DeviceConnection.cpp - Реалізація підключення до пристрію
 *
 * Інтегрує функціональність iTEUFDrs для підключення до USB пристроїв ITE IT1181-A1BA
 * згідно з аналізом Ghidra та реконструйованою логікою.
 */

#include "../include/Dialogs.h"
#include "../include/Utilities.h"
#include "../include/iTEUFDrs.h"

/**
 * DeviceConnectionManager - управління підключенням до пристрою
 */
namespace DeviceConnectionManager {

/**
 * Підключитися до пристрою
 * Використовує iTEUFDrs для виявлення та ініціалізації пристроїв
 */
BOOL ConnectToDevice(CUrescueDlg* pDialog, iTEUFDrs* pDeviceManager) {
    if(! pDialog || ! pDeviceManager) {
        LogError("ConnectToDevice: Invalid parameters");
        return FALSE;
    }

    LogMessage("ConnectToDevice: Starting device connection process");

    // Встановити діалог як батьківський для зворотних викликів
    pDeviceManager->SetParentDialog(pDialog);

    // Перевірити ініціалізацію SDK
    if(! pDeviceManager->IsInitialized()) {
        LogError("ConnectToDevice: Device manager not initialized");
        pDialog->SetStatusText("Помилка: SDK не ініціалізовано");
        pDialog->ShowProgressBar(FALSE);
        return FALSE;
    }

    // Показати прогрес підключення
    pDialog->SetStatusText("Пошук пристроїв ITE...");
    pDialog->ShowProgressBar(TRUE);
    pDialog->SetProgress(10);

    // Спочатку запустити процес виявлення пристроїв
    char deviceDetectionResult = pDeviceManager->GetDeviceInfo();
    pDialog->SetProgress(50);

    if(deviceDetectionResult == 0) {
        LogError("ConnectToDevice: No compatible devices found");
        pDialog->SetStatusText("Пристрій не знайдено. Перевірте підключення USB.");
        pDialog->ShowProgressBar(FALSE);
        return FALSE;
    }

    // Отримати детальну інформацію про пристрій (const версія)
    const _DEVICE_INFO& deviceInfo = static_cast<const iTEUFDrs*>(pDeviceManager)->GetDeviceInfo();
    pDialog->SetProgress(80);

    // Перевірити, чи знайдено пристрій
    if(! deviceInfo.deviceFound) {
        LogError("ConnectToDevice: Device found but not accessible");
        pDialog->SetStatusText("Пристрій знайдено, але недоступний");
        pDialog->ShowProgressBar(FALSE);
        return FALSE;
    }

    // Сформувати повідомлення про успішне підключення
    CString connectionMessage;
    if(deviceInfo.volumes[0].deviceFound && deviceInfo.volumes[0].volumeLetter != 0) {
        connectionMessage.Format(
            "Підключено: ITE IT1181 (Диск %c:)\nКонтролер: %04X\nТип: %02X",
            deviceInfo.volumes[0].volumeLetter,
            deviceInfo.volumes[0].controllerType,
            deviceInfo.volumes[0].familyType);
    } else {
        connectionMessage = "Підключено: ITE IT1181 пристрій";
    }

    pDialog->SetStatusText(connectionMessage);
    pDialog->SetProgress(100);

    // Коротка затримка для відображення прогресу
    Sleep(500);
    pDialog->ShowProgressBar(FALSE);

    LogMessage("ConnectToDevice: Successfully connected to device");
    return TRUE;
}

/**
 * Відключитися від пристрою
 */
BOOL DisconnectFromDevice(CUrescueDlg* pDialog, iTEUFDrs* pDeviceManager) {
    if(! pDialog) {
        LogError("DisconnectFromDevice: Invalid dialog parameter");
        return FALSE;
    }

    LogMessage("DisconnectFromDevice: Disconnecting from device");

    pDialog->SetStatusText("Відключення від пристрою...");

    // Якщо менеджер пристроїв доступний, очистити зв'язок
    if(pDeviceManager) {
        pDeviceManager->SetParentDialog(nullptr);
    }

    pDialog->SetStatusText("Пристрій відключено");

    LogMessage("DisconnectFromDevice: Device disconnected successfully");
    return TRUE;
}

/**
 * Перевірити стан підключення
 */
BOOL IsDeviceConnected(iTEUFDrs* pDeviceManager) {
    if(! pDeviceManager) {
        return FALSE;
    }

    if(! pDeviceManager->IsInitialized()) {
        return FALSE;
    }

    const _DEVICE_INFO& deviceInfo = static_cast<const iTEUFDrs*>(pDeviceManager)->GetDeviceInfo();
    return deviceInfo.deviceFound && deviceInfo.isInitialized;
}

/**
 * Отримати інформацію про підключений пристрій
 */
CString GetDeviceConnectionInfo(iTEUFDrs* pDeviceManager) {
    if(! pDeviceManager || ! IsDeviceConnected(pDeviceManager)) {
        return "Пристрій не підключено";
    }

    const _DEVICE_INFO& deviceInfo = static_cast<const iTEUFDrs*>(pDeviceManager)->GetDeviceInfo();

    CString info;
    info.Format(
        "ITE IT1181 Flash Drive\n"
        "Статус: Підключено\n"
        "Диск: %c:\n"
        "Контролер: %04X\n"
        "Тип: %02X\n"
        "Готовність: %s",
        deviceInfo.volumes[0].volumeLetter,
        deviceInfo.volumes[0].controllerType,
        deviceInfo.volumes[0].familyType,
        deviceInfo.systemReady ? "Готовий" : "Не готовий");

    return info;
}

/**
 * Автоматичне підключення при старті
 */
BOOL AutoConnectDevice(CUrescueDlg* pDialog, iTEUFDrs* pDeviceManager) {
    LogMessage("AutoConnectDevice: Attempting automatic device connection");

    if(! pDialog || ! pDeviceManager) {
        LogError("AutoConnectDevice: Invalid parameters");
        return FALSE;
    }

    // Спробувати підключитися автоматично
    pDialog->SetStatusText("Автоматичний пошук пристроїв...");

    BOOL result = ConnectToDevice(pDialog, pDeviceManager);

    if(result) {
        LogMessage("AutoConnectDevice: Automatic connection successful");
        // Додати повідомлення про успішне автопідключення
        CString statusMsg = GetDeviceConnectionInfo(pDeviceManager);
        statusMsg += "\n(Автоматично підключено)";
        pDialog->SetStatusText(statusMsg);
    } else {
        LogMessage("AutoConnectDevice: Automatic connection failed");
        pDialog->SetStatusText("Готовий до підключення пристрою");
    }

    return result;
}

}  // namespace DeviceConnectionManager
