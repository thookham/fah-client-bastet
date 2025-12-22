# Folding@home Client

[![Build Status](https://github.com/foldingathome/fah-client-bastet/actions/workflows/build.yml/badge.svg)](https://github.com/foldingathome/fah-client-bastet/actions)
[![License](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows%20%7C%20macOS-lightgrey.svg)](https://foldingathome.org/start-folding/)

The **Folding@home Client** allows you to donate your computer's unused processing power to help scientists study proteins and find cures for diseases like Cancer, ALS, Parkinson's, and Alzheimer's.

This repository (`fah-client-bastet`) contains the open-source **backend** of the client software. It manages the download, execution, and upload of scientific work units.

The frontend interface resides in the [fah-web-client-bastet](https://github.com/foldingathome/fah-web-client-bastet) repository.

---

## 🚀 Features

* **High Performance**: Efficiently utilizes CPU and GPU resources.
* **Cross-Platform**: Runs natively on Linux, Windows, and macOS.
* **Secure**: Validates all work units with digital signatures.
* **Controllable**: Comprehensive remote control interface via WebSocket.

## 🛠️ Build & Install

### Prerequisites

You need `scons`, `git`, and a C++ compiler (GCC, Clang, or MSVC).

**Debian/Ubuntu:**

```bash
sudo apt update
sudo apt install -y scons git build-essential libssl-dev zlib1g-dev libbz2-dev liblz4-dev
```

### Building form Source

1. **Clone the repositories:**

    ```bash
    git clone https://github.com/cauldrondevelopmentllc/cbang
    git clone https://github.com/foldingathome/fah-client-bastet
    ```

2. **Compile:**

    ```bash
    export CBANG_HOME=$PWD/cbang
    scons -C cbang
    scons -C fah-client-bastet
    ```

3. **Package (Optional):**

    ```bash
    scons -C fah-client-bastet package
    ```

### Running the Client

After building, you can run the client directly:

```bash
./fah-client-bastet/fah-client --help
```

---

## 🧪 Testing

We maintain a suite of automated tests to ensure reliability.

### Unit Tests (C++)

Run the core logic tests using `scons`:

```bash
scons -C fah-client-bastet test
```

This now includes unit tests for `UnitMetrics` and `Group` logic.

### CLI Tests (Python)

Verify the `fahctl` command-line tool:

```bash
pip install websocket-client
export PYTHONPATH=$PYTHONPATH:$(pwd)/fah-client-bastet
python3 fah-client-bastet/tests/test_fahctl.py
```

---

## 📂 Directory Structure

* `src/`: Core C++ source code.
  * `src/fah/client/`: Main client logic (Unit management, Networking).
* `tests/`: Unit and integration tests.
* `scripts/`: Helper scripts, including `fahctl`.
* `install/`: Platform-specific installer files (NSIS, Debian control, etc.).

---

## 🤝 Contributing

Contributions are welcome! Please read `COMPILING.md` (if available) and ensure your code follows our style guidelines:

* **Formatting**: We use `.clang-format` (Google style).
* **Docs**: Public APIs should have Doxygen comments.

## 📄 License

This software is distributed under the [GNU General Public License v3.0](LICENSE).
