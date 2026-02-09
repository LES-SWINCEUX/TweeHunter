#ifndef PANNEAU_MENU_H
#define PANNEAU_MENU_H

#include <QResizeEvent>
#include <QWidget>
#include <QDir>

class PanneauMenu : public QWidget
{
    Q_OBJECT
public:
    explicit PanneauMenu(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        setAttribute(Qt::WA_TranslucentBackground);
        setMouseTracking(true);
    }

    virtual ~PanneauMenu() = default;

signals:
    void demanderJouer();
    void demanderScores();
    void demanderOptions();
    void demanderQuitter();
    void demanderRetour();

protected:
    void initialiserPanneau()
    {
        creer();
        positionner();
    }

    virtual void creer() = 0;
    virtual void positionner() = 0;

    void resizeEvent(QResizeEvent* e) override
    {
        QWidget::resizeEvent(e);
        positionner();
    }
};

#endif