import unittest
from pathlib import Path

from tools.awdl_reconnect_recovery import AWDLRecoveryModel


class AWDLReconnectRecoveryTests(unittest.TestCase):
    def test_driver_stops_owned_listener_before_reassociation(self):
        source = (Path(__file__).resolve().parents[1] / "itlwm/hal_iwx/ItlIwx.cpp").read_text()
        self.assertIn(
            "awdlCoexAdded || awdlCoexAddingGen == sc->sc_generation || awdlCoexTxStaAdded",
            source,
        )

    def test_run_to_auth_stops_listener_once(self):
        model = AWDLRecoveryModel(listener_active=True)

        model.on_state_change("RUN", "AUTH", 7)
        model.on_state_change("AUTH", "ASSOC", 7)

        self.assertEqual(model.stop_count, 1)
        self.assertFalse(model.listener_active)

    def test_failed_retry_does_not_start_listener(self):
        model = AWDLRecoveryModel(listener_active=True)
        model.on_state_change("RUN", "AUTH", 7)

        model.on_state_change("AUTH", "AUTH", 8)
        model.on_state_change("AUTH", "ASSOC", 8)

        self.assertEqual(model.start_count, 0)
        self.assertFalse(model.listener_active)

    def test_return_to_run_starts_listener_once(self):
        model = AWDLRecoveryModel(listener_active=True)
        model.on_state_change("RUN", "AUTH", 7)
        model.on_state_change("AUTH", "ASSOC", 7)
        model.on_state_change("ASSOC", "RUN", 7)
        model.on_state_change("RUN", "RUN", 7)

        self.assertEqual(model.start_count, 1)
        self.assertTrue(model.listener_active)

    def test_stale_generation_is_ignored(self):
        model = AWDLRecoveryModel(listener_active=True)
        model.on_state_change("RUN", "AUTH", 7)
        model.on_state_change("AUTH", "RUN", 6)

        self.assertEqual(model.start_count, 0)
        self.assertFalse(model.listener_active)

    def test_run_to_run_is_noop(self):
        model = AWDLRecoveryModel(listener_active=True)
        model.on_state_change("RUN", "RUN", 7)

        self.assertEqual(model.stop_count, 0)
        self.assertEqual(model.start_count, 0)


if __name__ == "__main__":
    unittest.main()
