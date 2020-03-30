/*
 * Copyright (C) 2013 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Greeter.h"
#include "PromptsModel.h"

#include <QCoreApplication>
#include <QDBusInterface>
#include <QDBusReply>
#include <QSignalSpy>
#include <QQuickItem>
#include <QQuickView>
#include <QtTestGui>
#include <paths.h>

class GreeterDBusTest : public QObject
{
    Q_OBJECT

Q_SIGNALS:
    void PropertiesChangedRelay(const QString &interface, const QVariantMap &changed, const QStringList &invalidated);

private Q_SLOTS:

    void initTestCase()
    {
        // Qt doesn't like us connecting to PropertiesChanged using normal
        // SIGNAL method, because QtDBus doesn't know about PropertiesChanged.
        // So we connect the hard way for the benefit of any tests that want
        // to watch.
        QDBusConnection::sessionBus().connect(
            "com.canonical.UnityGreeter",
            "/",
            "org.freedesktop.DBus.Properties",
            "PropertiesChanged",
            this,
            SIGNAL(PropertiesChangedRelay(const QString&, const QVariantMap&, const QStringList&)));
        QDBusConnection::sessionBus().connect(
            "com.canonical.UnityGreeter",
            "/list",
            "org.freedesktop.DBus.Properties",
            "PropertiesChanged",
            this,
            SIGNAL(PropertiesChangedRelay(const QString&, const QVariantMap&, const QStringList&)));
    }

    void init()
    {
        view = new QQuickView();
        view->setSource(QUrl::fromLocalFile(testDataDir() + "/plugins/LightDM/IntegratedLightDM/greeter.qml"));
        greeter = dynamic_cast<Greeter*>(view->rootObject()->property("greeter").value<QObject*>());
        QVERIFY(greeter);
        QVERIFY(greeter->authenticationUser() == "");
        view->show();
        QTest::qWaitForWindowExposed(view);

        dbusMain = new QDBusInterface("com.canonical.UnityGreeter",
                                      "/",
                                      "com.canonical.UnityGreeter",
                                      QDBusConnection::sessionBus(), view);
        QVERIFY(dbusMain->isValid());

        dbusList = new QDBusInterface("com.canonical.UnityGreeter",
                                      "/list",
                                      "com.canonical.UnityGreeter.List",
                                      QDBusConnection::sessionBus(), view);
        QVERIFY(dbusList->isValid());
    }

    void cleanup()
    {
        delete view;
        delete greeter; // reset singleton
    }

    void testGetActiveEntry()
    {
        greeter->authenticate("has-password");

        QDBusReply<QString> reply = dbusList->call("GetActiveEntry");
        QVERIFY(reply.isValid());
        QVERIFY(reply.value() == "has-password");
    }

    void testSetActiveEntry()
    {
        QSignalSpy spy(greeter, &Greeter::requestAuthenticationUser);
        QDBusReply<void> reply = dbusList->call("SetActiveEntry", "has-password");
        QVERIFY(reply.isValid());
        spy.wait();

        QCOMPARE(spy.count(), 1);
        QList<QVariant> arguments = spy.takeFirst();
        QVERIFY(arguments.at(0).toString() == "has-password");
    }

    void testEntrySelectedSignal()
    {
        QSignalSpy spy(dbusList, SIGNAL(EntrySelected(QString)));
        greeter->authenticate("has-password");
        spy.wait();

        QCOMPARE(spy.count(), 1);
        QList<QVariant> arguments = spy.takeFirst();
        QVERIFY(arguments.at(0).toString() == "has-password");
    }

    void testActiveEntryGet()
    {
        greeter->authenticate("has-password");
        QVERIFY(dbusList->property("ActiveEntry").toString() == "has-password");
    }

    void testActiveEntrySet()
    {
        QSignalSpy spy(greeter, &Greeter::requestAuthenticationUser);
        QVERIFY(dbusList->setProperty("ActiveEntry", "has-password"));
        spy.wait();

        QCOMPARE(spy.count(), 1);
        QList<QVariant> arguments = spy.takeFirst();
        QVERIFY(arguments.at(0).toString() == "has-password");
    }

    void testActiveEntryChanged()
    {
        QSignalSpy spy(this, &GreeterDBusTest::PropertiesChangedRelay);

        greeter->authenticate("has-password");

        // EntryIsLocked's default is true, thus only single property change.
        QTRY_COMPARE(spy.count(), 1);

        QList<QVariant> arguments = spy[0];
        QVERIFY(arguments.at(0).toString() == "com.canonical.UnityGreeter.List");
        QVERIFY(arguments.at(1).toMap().contains("ActiveEntry"));
        QVERIFY(arguments.at(1).toMap()["ActiveEntry"] == "has-password");
    }

    void testEntryIsLockedGet()
    {
        QVERIFY(dbusList->property("EntryIsLocked").toBool());

        greeter->authenticate("no-password");
        QTRY_VERIFY(!dbusList->property("EntryIsLocked").toBool());

        QSignalSpy promptSpy(greeter->promptsModel(), &PromptsModel::countChanged);
        QSignalSpy successSpy(greeter, &Greeter::loginSuccess);

        greeter->authenticate("has-password");
        QTRY_VERIFY(dbusList->property("EntryIsLocked").toBool());
        // Make sure that EntryIsLocked stays `true` after authenticated.
        // Regression test for https://github.com/ubports/ubuntu-touch/issues/1406
        promptSpy.wait();
        greeter->respond(QStringLiteral("password"));
        successSpy.wait();
        QVERIFY(dbusList->property("EntryIsLocked").toBool());
    }

    void testEntryIsLockedChanged()
    {
        QSignalSpy spy(this, &GreeterDBusTest::PropertiesChangedRelay);

        greeter->authenticate("no-password");

        // ActiveEntry, then EntryIsLocked=false
        QTRY_COMPARE(spy.count(), 2);

        QList<QVariant> arguments = spy[1];
        QVERIFY(arguments.at(0).toString() == "com.canonical.UnityGreeter.List");
        QVERIFY(arguments.at(1).toMap().contains("EntryIsLocked"));
        QVERIFY(arguments.at(1).toMap()["EntryIsLocked"] == false);
    }

    void testIsActive()
    {
        QVERIFY(!greeter->isActive());
        QVERIFY(!dbusMain->property("IsActive").toBool());

        QSignalSpy spy(this, &GreeterDBusTest::PropertiesChangedRelay);
        greeter->setIsActive(true);
        spy.wait();

        QVERIFY(greeter->isActive());
        QVERIFY(dbusMain->property("IsActive").toBool());

        QCOMPARE(spy.count(), 1);
        QList<QVariant> arguments = spy.takeFirst();
        QCOMPARE(arguments.at(0).toString(), QString("com.canonical.UnityGreeter"));
        QVERIFY(arguments.at(1).toMap().contains("IsActive"));
        QVERIFY(arguments.at(1).toMap()["IsActive"].toBool());
    }

    void testShowGreeter()
    {
        // Just confirm the call exists and doesn't fail
        QDBusReply<void> reply = dbusMain->call("ShowGreeter");
        QVERIFY(reply.isValid());
    }

private:
    QQuickView *view;
    Greeter *greeter;
    QDBusInterface *dbusMain;
    QDBusInterface *dbusList;
};

QTEST_MAIN(GreeterDBusTest)

#include "dbus.moc"
