# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.


"""Tests for the Shell"""

from __future__ import absolute_import

from unity8.shell.tests import Unity8TestCase
from unity8.shell.tests.helpers import with_lightdm_mock

from autopilot.matchers import Eventually
from autopilot.platform import model
import logging
from testtools import skipUnless
from testtools.matchers import Equals


logger = logging.getLogger(__name__)


class TestLockscreen(Unity8TestCase):

    """Tests for the lock screen."""

    @with_lightdm_mock("single-pin")
    def test_can_unlock_pin_screen(self):
        """Must be able to unlock the PIN entry lock screen."""
        self.launch_unity()
        greeter = self.main_window.get_greeter()
        greeter.unlock()

        lockscreen = self._wait_for_lockscreen()
        self._enter_pincode("1234")

        self.assertThat(lockscreen.shown, Eventually(Equals(False)))

    @skipUnless(model() == 'Desktop', "Passphrase applicable to desktop only.")
    @with_lightdm_mock("single-passphrase")
    def test_can_unlock_passphrase_screen(self):
        """Must be able to unlock the passphrase entry screen."""
        self.launch_unity()
        greeter = self.main_window.get_greeter()
        greeter.unlock()

        lockscreen = self._wait_for_lockscreen()
        self._enter_passphrase("password")

        self.assertThat(lockscreen.shown, Eventually(Equals(False)))

    @with_lightdm_mock("single-pin")
    def test_pin_screen_wrong_code(self):
        """Entering the wrong pin code must not dismiss the lock screen."""
        self.launch_unity()
        greeter = self.main_window.get_greeter()
        greeter.unlock()

        lockscreen = self._wait_for_lockscreen()
        self._enter_pincode("4321")

        pinentryField = self.main_window.get_pinentryField()
        self.assertThat(pinentryField.text, Eventually(Equals("")))
        self.assertThat(lockscreen.shown, Eventually(Equals(True)))

    @skipUnless(model() == 'Desktop', "Passphrase applicable to desktop only.")
    @with_lightdm_mock("single-passphrase")
    def test_passphrase_screen_wrong_password(self):
        """Entering the wrong password must not dismiss the lock screen."""
        self.launch_unity()
        greeter = self.main_window.get_greeter()
        greeter.unlock()

        lockscreen = self._wait_for_lockscreen()
        self._enter_passphrase("foobar")

        pinentryField = self.main_window.get_pinentryField()
        self.assertThat(pinentryField.text, Eventually(Equals("")))
        self.assertThat(lockscreen.shown, Eventually(Equals(True)))

    def _wait_for_lockscreen(self):
        """Wait for the lock screen to load, and return it."""
        pinPadLoader = self.main_window.get_pinPadLoader();
        self.assertThat(pinPadLoader.progress, Eventually(Equals(1)))
        lockscreen = self.main_window.get_lockscreen();
        self.assertThat(lockscreen.shown, Eventually(Equals(True)))
        return lockscreen

    def _enter_pincode(self, code):
        """Enter code 'code' into the single-pin lightdm pincode entry
        screen.

        :param code: must be a string of numeric characters.
        :raises: TypeError if code is not a string.
        :raises: ValueError if code contains non-numeric characters.

        """

        if not isinstance(code, basestring):
            raise TypeError("'code' parameter must be a string.")
        for num in code:
            if not num.isdigit():
                raise ValueError(
                    "'code' parameter contains non-numeric characters."
                )
            self.touch.tap_object(self.main_window.get_pinPadButton(int(num)))

    def _enter_passphrase(self, passphrase):
        """Enter the password specified in 'passphrase' into the password entry
        field.

        :param passphrase: The string you want to enter.
        :raises: TypeError if passphrase is not a string.

        """
        if not isinstance(passphrase, basestring):
            raise TypeError("'passphrase' parameter must be a string.")

        pinentryField = self.main_window.get_pinentryField()
        self.touch.tap_object(pinentryField)
        self.keyboard.type(passphrase)
        self.keyboard.type("\n")



