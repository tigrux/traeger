# SPDX-License-Identifier: BSL-1.0

from skbuild import setup

setup(
    name="traeger",
    version="0.2.0",
    description="A minimal and portable Actor System for C++ and Python",
    author="tigrux@gmail.com",
    license="BSL-1.0",
    packages=["traeger"],
    python_requires=">=3.8",
    cmake_args=["-DCALL_FROM_SETUP_PY:BOOL=ON"],
)
