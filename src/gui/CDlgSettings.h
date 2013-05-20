#ifndef CDLGSETTINGS_H
#define CDLGSETTINGS_H

#include <QDialog>
#include <CSettings.h>

namespace Ui {
class CDlgSettings;
}

class CDlgSettings : public QDialog
{
    Q_OBJECT
    
public:
    explicit CDlgSettings(QWidget *parent = 0,CSettings *settings=0);
    ~CDlgSettings();
    void addInputSoundDevices(QHash<QString, int> value);
    void addOutputSoundDevices(QHash<QString, int> value);
    void initialize();

public slots:
    void accept();

private:
    Ui::CDlgSettings *ui;
    CSettings *settings;
};

#endif // CDLGSETTINGS_H
