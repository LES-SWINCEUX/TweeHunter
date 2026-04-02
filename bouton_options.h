#ifndef BOUTON_OPTIONS
#define BOUTON_OPTIONS

#include <QPushButton>
#include <QPainter>
#include <QPaintEvent>
#include <QLinearGradient>
#include <QFontMetrics>

class BoutonOptions : public QPushButton
{
    Q_OBJECT

public:
    enum class Theme {
        Rouge,
        Vert,
        Bleu
    };

    BoutonOptions(const QString& titre, const QString& sousTitre, Theme theme, QWidget* parent = nullptr);

    void setSelected(bool selected);
    bool isSelected() const { return m_selected; }

    void setTitre(const QString& titre);
    void setSousTitre(const QString& sousTitre);
    void setTheme(Theme theme);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QString m_titre;
    QString m_sousTitre;
    Theme m_theme = Theme::Rouge;
    bool m_selected = false;
};

#endif