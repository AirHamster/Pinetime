#pragma once
#include <FreeRTOS.h>
#include <task.h>
#include <drivers/St7789.h>
#include <drivers/SpiMaster.h>
#include <Components/Gfx/Gfx.h>
#include <bits/unique_ptr.h>
#include <queue.h>
#include <Components/Battery/BatteryController.h>
#include <Components/Brightness/BrightnessController.h>
#include <Components/Ble/BleController.h>
#include <Components/DateTime/DateTimeController.h>
#include "../drivers/Cst816s.h"
#include "LittleVgl.h"
#include <date/date.h>
#include <DisplayApp/Screens/Clock.h>
#include <drivers/Watchdog.h>
#include <DisplayApp/Screens/Modal.h>
#include <Components/Ble/NotificationManager.h>
#include <Components/FirmwareValidator/FirmwareValidator.h>
#include "TouchEvents.h"
#include "Apps.h"


namespace Pinetime {
  namespace System {
    class SystemTask;
  };
  namespace Applications {
    class DisplayApp {
      public:
        enum class States {Idle, Running};
        enum class Messages : uint8_t {GoToSleep, GoToRunning, UpdateDateTime, UpdateBleConnection, UpdateBatteryLevel, TouchEvent, ButtonPushed,
            NewNotification, BleFirmwareUpdateStarted };

        enum class FullRefreshDirections { None, Up, Down };
        enum class TouchModes { Gestures, Polling };

        DisplayApp(Drivers::St7789 &lcd, Components::LittleVgl &lvgl, Drivers::Cst816S &,
                   Controllers::Battery &batteryController, Controllers::Ble &bleController,
                   Controllers::DateTime &dateTimeController, Drivers::WatchdogView &watchdog,
                   System::SystemTask &systemTask,
                   Pinetime::Controllers::NotificationManager& notificationManager);
        void Start();
        void PushMessage(Messages msg);

        void StartApp(Apps app);

        void SetFullRefresh(FullRefreshDirections direction);
        void SetTouchMode(TouchModes mode);

      private:
        TaskHandle_t taskHandle;
        static void Process(void* instance);
        void InitHw();
        Pinetime::Drivers::St7789& lcd;
        Pinetime::Components::LittleVgl& lvgl;
        void Refresh();

        States state = States::Running;
        void RunningState();
        void IdleState();
        QueueHandle_t msgQueue;

        static constexpr uint8_t queueSize = 10;
        static constexpr uint8_t itemSize = 1;

        Pinetime::Controllers::Battery &batteryController;
        Pinetime::Controllers::Ble &bleController;
        Pinetime::Controllers::DateTime& dateTimeController;
        Pinetime::Drivers::WatchdogView& watchdog;

        Pinetime::Drivers::Cst816S& touchPanel;
        TouchEvents OnTouchEvent();

        std::unique_ptr<Screens::Screen> currentScreen;

        bool isClock = true;

        Pinetime::System::SystemTask& systemTask;
        Apps nextApp = Apps::None;
        bool onClockApp = false; // TODO find a better way to know that we should handle gestures and button differently for the Clock app.
        Controllers::BrightnessController brightnessController;
        std::unique_ptr<Screens::Modal> modal;
        Pinetime::Controllers::NotificationManager& notificationManager;
        Pinetime::Controllers::FirmwareValidator validator;
        TouchModes touchMode = TouchModes::Gestures;
    };
  }
}


