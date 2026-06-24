#ifndef LANGUAGE_MANAGER_HPP
#define LANGUAGE_MANAGER_HPP

#include <QString>
#include <QObject>
#include <map>

/**
 * @brief Enumeration for supported languages
 */
enum class Language {
    English,
    Polish
};

/**
 * @class LanguageManager
 * @brief Singleton class that manages application language and translations.
 * * Provides a centralized way to get translated strings throughout the application.
 * Supports English and Polish. Emits signals when language changes to update UI.
 */
class LanguageManager : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Get the singleton instance of LanguageManager
     * @return LanguageManager instance
     */
    static LanguageManager& getInstance();
    
    /**
     * @brief Get translated string for given key
     * @param[in] key - the translation key (e.g., "home_welcome_title")
     * @return Translated string in current language, or key if not found
     */
    QString translate(const QString &key) const;
    
    /**
     * @brief Set the current language and emit languageChanged signal
     * @param[in] lang - the language to switch to
     */
    void setLanguage(Language lang);
    
    /**
     * @brief Get the current active language
     * @return Current Language
     */
    Language getCurrentLanguage() const;
    
    /**
     * @brief Get language code as string
     * @return "en" for English, "pl" for Polish
     */
    QString getLanguageCode() const;

signals:
    /**
     * @brief Signal emitted when language is changed.
     * Connected widgets should refresh their text in this signal handler.
     */
    void languageChanged();

private:
    /**
     * @brief Private constructor for singleton pattern
     */
    LanguageManager();
    
    /**
     * @brief Delete copy constructor
     */
    LanguageManager(const LanguageManager&) = delete;
    
    /**
     * @brief Delete copy assignment operator
     */
    LanguageManager& operator=(const LanguageManager&) = delete;
    
    Language current_language_; /**< Stores the currently active application language. */
    
    /**
     * @brief Initialize all translation strings
     */
    void initializeTranslations();
    
    /**< Translation storage: key -> (English, Polish) */
    std::map<QString, std::pair<QString, QString>> translations_;
};

#endif // LANGUAGE_MANAGER_HPP