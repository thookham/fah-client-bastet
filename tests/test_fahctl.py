import unittest
from unittest.mock import patch, MagicMock
import sys
import io
import json
import os

# Ensure we can import the script.
# Since fahctl doesn't have a .py extension, we might need to load it dynamically
# or just assume it's in the path if we were running it.
# For testing logic, it's best if the script code was in a function we could import.
# However, the script is written as a top-level script.
# We will mock sys.argv and run the script using runpy or similar, or better:
# Refactor fahctl slightly to make it testable (put main logic in a function).

# Let's see if we can just modify the script slightly to be importable or testable first?
# The script runs code at top level.

# Plan B: Create a wrapper that imports it? No, it executes immediately.
# I will modify fahctl to have a main() function.

class TestFahCtl(unittest.TestCase):
    @patch('sys.stdout', new_callable=io.StringIO)
    @patch('sys.stderr', new_callable=io.StringIO)
    @patch('websocket.create_connection')
    def test_fold_command(self, mock_create_connection, mock_stderr, mock_stdout):
        # Mock WebSocket
        mock_ws = MagicMock()
        mock_create_connection.return_value = mock_ws
        
        # Initial state response
        mock_ws.recv.return_value = json.dumps({"groups": {"0": {}}})
        
        from scripts import fahctl # calling the refactored version

        # Run with arguments
        with patch.object(sys, 'argv', ['fahctl', 'fold']):
            fahctl.main()

        # Verify send was called with correct json
        expected_msg = json.dumps({'cmd': 'state', 'state': 'fold'})
        mock_ws.send.assert_called_with(expected_msg)

    @patch('websocket.create_connection')
    def test_group_command(self, mock_create_connection):
        mock_ws = MagicMock()
        mock_create_connection.return_value = mock_ws
        mock_ws.recv.return_value = json.dumps({"groups": {"gpu": {}}})
        
        from scripts import fahctl

        with patch.object(sys, 'argv', ['fahctl', 'pause', 'gpu']):
            fahctl.main()

        expected_msg = json.dumps({'cmd': 'state', 'state': 'pause', 'group': 'gpu'})
        mock_ws.send.assert_called_with(expected_msg)

if __name__ == '__main__':
    unittest.main()
