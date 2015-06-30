/*
 * Copyright (C) 2013, 2015 Canonical, Ltd.
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
 *
 * Author: Michael Zanetti <michael.zanetti@canonical.com>
 */

#include "audio.h"

Audio::Audio(QObject* parent):
    QObject(parent),
    m_playbackState(StoppedState),
    m_playlist(0)
{
    qsrand(time(nullptr));
    m_timer.setInterval(1000);
    connect(&m_timer, SIGNAL(timeout()), SLOT(timerEvent()));

    m_position = 0;
    Q_EMIT positionChanged(m_position);

    m_duration = (qrand() % 20000) + 10000;
    Q_EMIT durationChanged(m_duration);
}

QUrl Audio::source() const
{
    return m_source;
}

void Audio::setSource(const QUrl &source)
{
    if (source == m_source && m_playlist == NULL)
        return;

    m_playlist = NULL;
    Q_EMIT playlistChanged();

    m_source = source;
    Q_EMIT sourceChanged(source);

    m_position = 0;
    Q_EMIT positionChanged(m_position);

    m_duration = (qrand() % 20000) + 10000;
    Q_EMIT durationChanged(m_duration);
}

Audio::PlaybackState Audio::playbackState() const
{
    return m_playbackState;
}

int Audio::position() const
{
    return m_position;
}

int Audio::duration() const
{
    return m_duration;
}

QString Audio::errorString() const
{
    return QString();
}

void Audio::pause()
{
    if (m_playbackState == PlayingState) {
        m_playbackState = PausedState;
        Q_EMIT playbackStateChanged(m_playbackState);
        m_timer.stop();
    }
}

void Audio::play()
{
    if (m_playbackState != PlayingState) {
        m_playbackState = PlayingState;
        Q_EMIT playbackStateChanged(m_playbackState);

        m_timer.start();
    }
}

void Audio::stop()
{
    if (m_playbackState != StoppedState) {
        m_playbackState = StoppedState;
        Q_EMIT playbackStateChanged(m_playbackState);
        m_timer.stop();
        m_position = 0;
        Q_EMIT positionChanged(m_position);
    }
}

void Audio::timerEvent()
{
    if (m_position + 1000 < m_duration) {
        m_position += 1000;
        Q_EMIT positionChanged(m_position);
    } else {
        stop();
    }
}

Audio::AudioRole Audio::audioRole() const
{
    return Audio::MultimediaRole;
}

void Audio::setAudioRole(Audio::AudioRole audioRole)
{
    Q_UNUSED(audioRole);
}

DeclarativePlaylist *Audio::playlist() const
{
    return m_playlist;
}

void Audio::setPlaylist(DeclarativePlaylist *playlist)
{
    if (playlist == m_playlist && m_source.isEmpty())
        return;

    m_source.clear();
    Q_EMIT sourceChanged(m_source);
    m_playlist = playlist;
    Q_EMIT playlistChanged();
}
