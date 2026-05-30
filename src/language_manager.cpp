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
    return key; // Return key if not found
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
        {"Welcome to the WDS Project Home Page!", "Witamy na stronie głównej projektu WDS!"};
    
    translations_["home_subtitle"] = 
        {"Authors: Radosław Mijał & Mateusz Duda", "Autorzy: Radosław Mijał & Mateusz Duda"};
    
    translations_["home_language_label"] = 
        {"Language:", "Język:"};
    
    translations_["home_english_button"] = 
        {"English", "English"};
    
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
        {"YAW", "KĄT"};
    
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
    
    translations_["slam_page"] = 
        {"SLAM Page EN", "SLAM Page PL"};
    
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
