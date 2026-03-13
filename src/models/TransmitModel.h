#pragma once

#include <QObject>
#include <QMap>
#include <QString>
#include <QStringList>

namespace AetherSDR {

// ATU tune status values (from FlexLib ATUTuneStatus enum).
enum class ATUStatus {
    None,
    NotStarted,
    InProgress,
    Bypass,
    Successful,
    OK,
    FailBypass,
    Fail,
    Aborted,
    ManualBypass
};

// State model for the radio's transmit parameters and internal ATU.
//
// Transmit status arrives via TCP as "transmit rfpower=93 tunepower=38 ..."
// after "sub tx all".  ATU status arrives as "atu status=TUNE_SUCCESSFUL ...".
//
// Commands use "transmit set ..." for power, "transmit tune <0|1>" for tune,
// "xmit <0|1>" for MOX, and "atu ..." for ATU control.
class TransmitModel : public QObject {
    Q_OBJECT

public:
    explicit TransmitModel(QObject* parent = nullptr);

    // ── Transmit getters ────────────────────────────────────────────────────
    int     rfPower()       const { return m_rfPower; }
    int     tunePower()     const { return m_tunePower; }
    bool    isTuning()      const { return m_tune; }
    bool    isMox()         const { return m_mox; }

    // ── ATU getters ─────────────────────────────────────────────────────────
    bool      atuEnabled()      const { return m_atuEnabled; }
    ATUStatus atuStatus()       const { return m_atuStatus; }
    bool      memoriesEnabled() const { return m_memoriesEnabled; }
    bool      usingMemory()     const { return m_usingMemory; }

    // ── Profile getters ─────────────────────────────────────────────────────
    QStringList profileList()    const { return m_profileList; }
    QString     activeProfile()  const { return m_activeProfile; }

    // ── Status parsing (called from RadioModel) ─────────────────────────────
    void applyTransmitStatus(const QMap<QString, QString>& kvs);
    void applyAtuStatus(const QMap<QString, QString>& kvs);
    void setProfileList(const QStringList& profiles);
    void setActiveProfile(const QString& profile);

    // ── Command methods (emit commandReady) ─────────────────────────────────
    void setRfPower(int power);
    void setTunePower(int power);
    void startTune();
    void stopTune();
    void setMox(bool on);
    void atuStart();
    void atuBypass();
    void setAtuMemories(bool on);
    void loadProfile(const QString& name);

signals:
    void stateChanged();
    void tuneChanged(bool tuning);
    void moxChanged(bool mox);
    void atuStateChanged();
    void profileListChanged();
    void commandReady(const QString& cmd);

private:
    static ATUStatus parseAtuTuneStatus(const QString& s);

    // Transmit state
    int  m_rfPower{100};
    int  m_tunePower{10};
    bool m_tune{false};
    bool m_mox{false};

    // ATU state
    bool      m_atuEnabled{false};
    ATUStatus m_atuStatus{ATUStatus::None};
    bool      m_memoriesEnabled{false};
    bool      m_usingMemory{false};

    // TX profiles
    QStringList m_profileList;
    QString     m_activeProfile;
};

} // namespace AetherSDR
