#ifndef WRITEREGISTERMODEL_H
#define WRITEREGISTERMODEL_H

#include <QAbstractItemModel>
#include <QBitArray>
#include <QObject>

class WriteRegisterModel : public QAbstractTableModel
{
    Q_OBJECT

public:
	WriteRegisterModel(const QStringList& participantList, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

public slots:
    void setStartAddress(int address);
    void setNumberOfValues(const QString &number);
    void updateParticipantList();

signals:
    void updateViewport();

public:
    int m_number = 0;
    int m_address = 0;
    QBitArray m_coils;
    //    QVector<quint16> m_holdingRegisters;
    void setParticipantList(const QStringList &newParticipantList);

private:
	QStringList participantList;
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
	int RowCount;
};

#endif // WRITEREGISTERMODEL_H
