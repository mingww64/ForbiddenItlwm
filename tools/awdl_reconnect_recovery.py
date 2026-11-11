"""Pure state-transition oracle for AWDL station reconnect recovery."""


class AWDLRecoveryModel:
    """Model listener ownership across normal Wi-Fi state transitions."""

    _DOWNWARD_STATES = {"AUTH", "ASSOC", "SCAN"}

    def __init__(self, listener_active=False):
        self.listener_active = listener_active
        self.stop_count = 0
        self.start_count = 0
        self._generation = None

    def on_state_change(self, old_state, new_state, generation):
        if old_state == "RUN" and new_state in self._DOWNWARD_STATES:
            self._generation = generation
            if self.listener_active:
                self.listener_active = False
                self.stop_count += 1
            return

        if new_state == "RUN" and self._generation == generation:
            if not self.listener_active:
                self.listener_active = True
                self.start_count += 1
