#include "language_manager.hpp"

LanguageManager::LanguageManager() : current_language_(Language::English) {
    initializeTranslations();
}

LanguageManager& LanguageManager::getInstance() {
    static LanguageManager instance;
    return instance;
}

QString LanguageManager::translate(const QString &key) const {
    auto it = translations_.find(key);
    if (it != translations_.end()) {
        if (current_language_ == Language::English) {
            return it->second.first;
        } else {
            return it->second.second;
        }
    }
    return key; 
}

void LanguageManager::setLanguage(Language lang) {
    if (current_language_ != lang) {
        current_language_ = lang;
        emit languageChanged();
    }
}

Language LanguageManager::getCurrentLanguage() const {
    return current_language_;
}

QString LanguageManager::getLanguageCode() const {
    return (current_language_ == Language::English) ? "en" : "pl";
}

void LanguageManager::initializeTranslations() {
    // ===== WINDOW TITLE =====
    translations_["window_title"] = 
        {"Leo Rover Base - Qt GUI", "Leo Rover Base - Qt GUI"};
    
    // ===== HOME PAGE =====
    translations_["home_welcome_title"] = 
        {"Welcome to the LeoDash!", "Witamy na LeoDash!"};
    
    translations_["home_subtitle"] = 
        {"Authors: Radosław Mijał & Mateusz Duda", "Autorzy: Radosław Mijał & Mateusz Duda"};
    
    translations_["home_info_text"] = 
        {"<div style='text-align: left;'>"
         "<h3>Project Functionalities:</h3>"
         "<ul style='margin-left: 0; padding-left: 20px;'>" 
         "<li><b>Home:</b> Language selection and rover model.</li>"
         "<li><b>SLAM:</b> Laser and path visualization.</li>"
         "<li><b>IMU:</b> Telemetry and 2.5D view.</li>"
         "</ul></div>", 
         "<div style='text-align: left;'>"
         "<h3>Funkcjonalności projektu:</h3>"
         "<ul style='margin-left: 0; padding-left: 20px;'>" 
         "<li><b>Strona główna:</b> Wybór języka i model robota.</li>"
         "<li><b>SLAM:</b> Wizualizacja lasera i ścieżki.</li>"
         "<li><b>IMU:</b> Telemetria i widok 2.5D.</li>"
         "</ul></div>"};
    
    translations_["home_language_label"] = 
        {"Language:", "Język:"};
    
    translations_["home_english_button"] = 
        {"English", "Angielski"};
    
    translations_["home_polish_button"] = 
        {"Polish", "Polski"};
    
    // ===== WINDOW TABS =====
    translations_["tab_home"] = 
        {"Home", "Strona główna"};
    
    translations_["tab_slam"] = 
        {"SLAM", "SLAM"};
    
    translations_["tab_imu"] = 
        {"IMU", "IMU"};
    
    // ===== IMU PAGE =====
    translations_["imu_title"] = 
        {"LEO SYSTEM", "SYSTEM LEO"};
    
    translations_["imu_camera_waiting"] = 
        {"WAITING FOR IMAGE...", "OCZEKIWANIE NA OBRAZ..."};
    
    translations_["imu_pose_title"] = 
        {"POSE", "POZYCJA"};
    
    translations_["imu_velocity_title"] = 
        {"VELOCITY", "PRĘDKOŚĆ"};
    
    translations_["imu_x_label"] = 
        {"X", "X"};
    
    translations_["imu_y_label"] = 
        {"Y", "Y"};
    
    translations_["imu_yaw_label"] = 
        {"YAW", "YAW"};
    
    translations_["imu_linear_vel"] = 
        {"LIN", "LIN"};
    
    translations_["imu_angular_vel"] = 
        {"ANG", "KĄT"};
    
    translations_["imu_unit_meters"] = 
        {"m", "m"};
    
    translations_["imu_unit_ms"] = 
        {"m/s", "m/s"};
    
    translations_["imu_unit_degrees"] = 
        {"°", "°"};
    
    translations_["imu_unit_degps"] = 
        {"°/s", "°/s"};
    
    // ===== SLAM PAGE =====
    translations_["slam_title"] = 
        {"SLAM Visualization", "Wizualizacja SLAM"};
    
    // ===== CONTROL PAD =====
    translations_["control_forward"] = 
        {"Forward", "Lewo"};
    
    translations_["control_backward"] = 
        {"Backward", "Prawo"};
    
    translations_["control_left"] = 
        {"Left", "Przód"};
    
    translations_["control_right"] = 
        {"Right", "Tył"};
    
    translations_["control_stop"] = 
        {"Stop", "Stop"};
}
