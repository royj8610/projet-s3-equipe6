# -*- coding: utf-8 -*-
#!/usr/bin/env python3

from enum import StrEnum


class ANSI(StrEnum):
    """
    ANSI escape codes for coloring terminal output.

    _N.B._ Put `NORMAL` at the end of the colored string to reset the output color.
    """

    NORMAL = "\033[0m"
    RED = "\033[91m"
    GREEN = "\033[92m"
    YELLOW = "\033[93m"
