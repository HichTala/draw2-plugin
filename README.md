<div align="center">
    <p>
        <img src="https://raw.githubusercontent.com/HichTala/draw2/refs/heads/main/docs/assets/banner.png" alt="DRAW Banner">
    </p>


<div>

[![Web Interface](https://img.shields.io/badge/🌐_Web_Interface-Try_It_Now-blue)](https://hichtala.github.io/draw2)

[![DRAW2 Workflow](https://github.com/HichTala/draw2-plugin/actions/workflows/push.yaml/badge.svg)](https://github.com/HichTala/draw2-plugin/actions/workflows/push.yaml)
[![Licence](https://img.shields.io/pypi/l/ultralytics)](LICENSE)
[![Github](https://img.shields.io/badge/-github-181717?logo=github&labelColor=555)](https://github.com/HichTala/draw2)
[![Twitter](https://img.shields.io/badge/-twitter-000?logo=x&labelColor=555)](https://twitter.com/hichtala)
[![HuggingFace Downloads](https://img.shields.io/badge/dynamic/json?url=https%3A%2F%2Fhuggingface.co%2Fapi%2Fmodels%2FHichTala%2Fdraw2&query=%24.downloads&logo=huggingface&label=downloads&color=%23FFD21E)](https://huggingface.co/HichTala/draw2)
[![Medium](https://img.shields.io/badge/-Medium-12100E?style=flat&logo=medium&labelColor=555)](https://medium.com/@hich.tala.phd/how-i-trained-again-my-model-to-detect-and-recognise-a-wide-range-of-yu-gi-oh-cards-5c567a320b0a)
[![WandB](https://img.shields.io/badge/visualize_in-W%26B-yellow?logo=weightsandbiases&color=%23FFBE00)](https://wandb.ai/hich_/draw)

[🇫🇷 Français](readmes/README_fr.md) | [🇧🇷 Português](readmes/README_pt-br.md) | [🇯🇵 日本語](readmes/README_jp.md) | [🇪🇸 Español](readmes/README_es.md)

</div>

</div>

DRAW 2 (which stands for **D**etect and **R**ecognize **A** **W**ide range of cards version 2) is an object detector
trained to detect _Yu-Gi-Oh!_ cards in all types of images, and in particular in dueling images.

This project is the plugin part of the DRAW 2 system. It allows users to seamlessly integrate the
detector directly into their live streams or recorded videos; and those **without any particular technical skills**.
The plugin can display detected cards in real time for an enhanced viewing experience.
The python backend project is available [here](https://github.com/HichTala/draw2).

This project is licensed under the [GNU Affero General Public License v3.0](LICENCE); all contributions are welcome.

---

## <div align="center">📰 News</div>

> 🃏 **Latest card pool:** `CORI` --- last updated `13-06-2026`  
> 🔧 **Latest app version:** `0.2.1-beta` --- last updated `01-06-2026`

<table>
  <tr>
    <th>Date</th>
    <th>Type</th>
    <th>Description</th>
  </tr>
  <tr>
    <td><b>13-06-2026</b></td>
    <td>🃏 Card Pool</td>
    <td>Card pool updated --- now supports cards up to <i>Chaos Origins</i></td>
  </tr>
  <tr>
    <td><b>01-06-2026</b></td>
    <td>🔧 App Update</td>
    <td>Released version 0.2.1-beta --- <a href="https://github.com/HichTala/draw2-plugin/releases/tag/0.2.1">see release notes</a></td>
  </tr>
  <tr>
    <td><b>18-05-2026</b></td>
    <td>🃏 Card Pool</td>
    <td>Card pool updated --- now supports cards up to <i>Blazing Dominion</i></td>
  </tr>
  <tr>
    <td><b>06-04-2026</b></td>
    <td>🃏 Card Pool</td>
    <td>Card pool updated --- now supports cards up to <i>Maze of Muertos</i></td>
  </tr>
  <tr>
    <td><b>06-04-2026</b></td>
    <td>🔧 App Update</td>
    <td>Released version 0.2.0-beta --- <a href="https://github.com/HichTala/draw2-plugin/releases/tag/0.2.0">see release notes</a></td>
  </tr>
  <tr>
    <td><b>09-03-2025</b></td>
    <td>🔧 App Update</td>
    <td>Released version 0.1.5-alpha --- <a href="https://github.com/HichTala/draw2-plugin/releases/tag/0.1.5">see release notes</a></td>
  </tr>
  <tr>
    <td><b>24-08-2025</b></td>
    <td>🃏 Card Pool</td>
    <td>Card pool updated --- now supports cards up to <i>Justic Hunter</i></td>
  </tr>
</table>

---

## <div align="center">📄Documentation</div>

### 🛠️ Installation

Follow the installation instruction depending on your operating system so everything works smoothly:

<details open>
<summary>🪟 Windows</summary>

1. Download the plugin installer from this
   link: [DRAW2 Plugin Installer](https://github.com/HichTala/draw2-plugin/releases/download/0.2.1/draw2-plugin-installer.exe)

2. Run the installer and follow the on-screen instructions.

3. Once the installation is complete, launch OBS Studio. If everything is set up correctly, you should see in the
   `Docks` menu a new option called `Draw 2`. You can activate the dock and dock it wherever you want.

The installation is complete, you can enjoy detecting !

</details>

<details>
<summary>🐧 Linux</summary>

1. Download the plugin installer from this link: 
   [DRAW2 Plugin Installer](https://github.com/HichTala/draw2-plugin/releases/download/0.2.1/draw2-plugin-0.2.1-x86_64-linux-gnu.deb)

2. Either run the installer by double-clicking it, and click install _OR_ by launching the command:
   
   ```shell
   sudo apt install ./draw2-plugin-0.2.1-x86_64-linux-gnu.deb
   ```
   
3. Once the installation is complete, launch OBS Studio. If everything is set up correctly, you should see in the
   `Docks` menu a new option called `Draw 2`. You can activate the dock and dock it wherever you want. The installation
   is not complete yet, the plugin is installed, but you still need to install python backend, close OBS and follow next 
   steps.

4. If you already have a python installation you want to use you can pass this step. Any python installation with 
   `draw` package installed can work. We will use a self-contained CPython. A relocatable build from
   [python-build-standalone](https://github.com/astral-sh/python-build-standalone):

   ```shell
   # pick the install_only build for your arch (x86_64 for Intel and AMD cpu)
   curl -fL -o python.tar.gz https://github.com/astral-sh/python-build-standalone/releases/download/20261009/cpython-3.13.16+20261009-x86_64-unknown-linux-gnu-install_only.tar.gz
   mkdir -p ~/.draw2-runtime && tar -xzf python.tar.gz -C ~/.draw2-runtime
   ```
   
5. Install `draw` backend (make sure [git](https://git-scm.com/install/linux) is installed):
   
   ```shell
   ~/.draw2-runtime/python/bin/python -m pip install "git+https://github.com/HichTala/draw2@obs-plugin"
   ```

6. In the Draw 2 settings, set **Select Python installation** to the prefix folder
   (the one that contains `bin/` and `lib/`), e.g. `~/.draw2-runtime/python`.
   Expected layout:

   ```text
   <prefix>/bin/python
   <prefix>/lib/python3.13/site-packages/draw
   ```
   
The installation is complete, you can enjoy detecting !
</details>

<details>
<summary>🍏 MacOS</summary>

1. Download the plugin installer from this link: 
   [DRAW2 Plugin Installer]() (macOS build hasn't been released yet, we're currently working on it, it will be released
   soon, you can still run the plugin by building it from source)

2. Run the installer by double-clicking it, there you should see a popup telling you that apple could not verify the
   plugin, there you can click done, then go your settings a search for `Privacy & Security`, scroll down until you see
   `"draw2-plugin....pkg" was blocked to protect your Mac`, here click `Open Anyway` and `Open Anyway` again and
   follow the on-screen instructions.

3. Once the installation is complete, launch OBS Studio. If everything is set up correctly, you should see in the
   `Docks` menu a new option called `Draw 2`. You can activate the dock and dock it wherever you want. The installation
   is not complete yet, the plugin is installed, but you still need to install python backend, close OBS and follow next
   steps.

4. If you already have a python installation you want to use you can pass this step. Any python installation with
   `draw` package installed can work. We will use a self-contained CPython. A relocatable build from
   [python-build-standalone](https://github.com/astral-sh/python-build-standalone):

   ```shell
   # pick the install_only build for your arch (aarch64 for Apple Silicon)
   curl -fL -o python.tar.gz https://github.com/astral-sh/python-build-standalone/releases/download/20261009/cpython-3.13.16+20261009-aarch64-apple-darwin-install_only.tar.gz
   mkdir -p ~/.draw2-runtime && tar -xzf python.tar.gz -C ~/.draw2-runtime
   ```

5. Install `draw` backend (make sure [git](https://git-scm.com/install/mac) is installed):
   
   ```shell
   ~/.draw2-runtime/python/bin/python -m pip install "git+https://github.com/HichTala/draw2@obs-plugin"
   ```

6. In the Draw 2 settings, set **Select Python installation** to the prefix folder
   (the one that contains `bin/` and `lib/`), e.g. `~/.draw2-runtime/python`.
   Expected layout:

   ```text
   <prefix>/bin/python
   <prefix>/lib/python3.13/site-packages/draw
   ```
   
The installation is complete, you can enjoy detecting !

</details>

### 🚀 Usage

When the plugin is installed and the model weights are downloaded, you can launch OBS Studio.

1. Open the `Docks` menu and select `Draw 2` to activate the plugin dock.
2. In the Draw 2 dock, you can configure the plugin settings by clicking on the gear icon next to `Start DRAW` button:
   - **Select Python installation**: Path to the Python prefix that has the `draw` backend installed (the folder
     containing `bin/` and `lib/`). Must be a full Python install, not a virtualenv.
   - **Select Deck List**: Choose the deck list file that contains the cards you want to detect. 3 deck lists
     can be handled at the same time. To add new deck lists, you can click the `Open Folder` button and drag and drop
     your deck list files (in ydk format) into the opened folder.
   - **Minimum Out of Screen Time**: The minimum time a card just detected can be displayed again.
   - **Minimum Screen Time**: The minimum time a card is displayed.
   - **Confidence Threshold**: Set the minimum confidence level for card detection. Detections below this threshold
     will be ignored.
   - **Advanced features** (off by default): two optional detector-input tweaks that only affect what the detector
     sees, not your live output. Enable them here, then configure the values on the `Draw Display` source:
     - **Enable detector input crop** — adds **Crop (Left/Top/Right/Bottom, px)** fields to the source, to focus
       detection on the region where cards are placed.
     - **Enable 180° input rotation** — adds a **Rotate input 180°** toggle to the source, for a camera mounted
       upside down.
     - When either is enabled, the source also gains a **Preview detector input** toggle: turn it on to make the
       source render the cropped/rotated frame it feeds the detector (so you can dial in the crop right in the
       source's preview), then turn it off to go back to showing detected cards.
3. The plugin provide a new source called `Draw Display`. You can add it to your scene like any other source.
   This source will display the detected cards on the screen. You can choose what source/scene to detect cards from.
4. Click the `Start DRAW` button to start the detection process. The plugin will start detecting cards in real time
   and display them on the screen using the `Draw Display` source. The plugin start detecting from the moment you see
   the
   `Stop DRAW` button. If you don't see it something went wrong.
5. In the other case you can enjoy the plugin!

Here is a small overview :)

<div align="center">
    <img src="https://raw.githubusercontent.com/HichTala/draw2/refs/heads/main/docs/assets/overview.gif" width="960" height="540" />
</div>


### ⚙️ Building from source

<details>
<summary>🪟 Windows</summary>

Coming soon 👀

</details>
<details>
<summary>🐧 Linux</summary>

#### Building the plugin from source

1. Install the build prerequisites with [Homebrew](https://brew.sh):

   ```bash
   sudo apt update
   sudo apt install -y cmake
   ```

2. Clone the source:

   ```shell
   git clone https://github.com/HichTala/draw2-plugin
   cd draw2-plugin
   ```

3. Configure and build:

   ```bash
   cmake --preset linux
   cmake --build build_linux --config RelWithDebInfo
   ```

4. The built plugin bundle is produced at:

   ```
   build_linux/RelWithDebInfo/
   ```

5. Install it by copying the bundle into your OBS plugins folder, then restart OBS:

   ```bash
   # adapt the path depending on your obs installation
   sudo cp -r release/RelWithDebInfo/* /usr/
   ```

   Once OBS restarts, the plugin appears as `Draw 2` in the `Docks` menu.

#### Setting up the Python backend

The plugin runs the `draw` backend as a **separate Python process**, it does not
embed an interpreter. You provide a Python installation that has the `draw`
package, and the plugin launches `<prefix>/bin/python` and communicates with it
over shared memory.

> The Python you point to just needs `bin/python` and the `draw` package
> importable. **Any reasonably recent Python 3 works, it does not have to match
> the plugin's build**. A self-contained CPython is the easiest to set up if you don't have
> a python installation already

- You can refer to this repo to get a self-contained CPython [python-build-standalone](https://github.com/astral-sh/python-build-standalone)
- To install the `draw` backend use the **`obs-plugin` branch**, that is the one that exposes the entry point
  the plugin launches (the default `main` branch is the standalone CLI):
   ```shell
   ~/.draw2-runtime/python/bin/python -m pip install "git+https://github.com/HichTala/draw2@obs-plugin"
   ```
- In the Draw 2 settings, set **Select Python installation** to the prefix folder
  (the one that contains `bin/` and `lib/`), e.g. `~/.draw2-runtime/python`. Expected layout:

   ```text
   <prefix>/bin/python
   <prefix>/lib/python3.13/site-packages/draw
   ```

</details>

<details>
<summary>🍏 MacOS</summary>

#### Building the plugin from source

1. Install the build prerequisites with [Homebrew](https://brew.sh):

   ```bash
   brew install cmake ccache coreutils jq xcbeautify
   ```

   You also need a recent **Xcode** (with its command line tools).

2. Clone the source:

   ```shell
   git clone https://github.com/HichTala/draw2-plugin
   cd draw2-plugin
   ```

3. Configure and build:

   ```bash
   cmake --preset macos
   cmake --build build_macos --config RelWithDebInfo
   ```

4. The built plugin bundle is produced at:

   ```
   build_macos/RelWithDebInfo/draw2-plugin.plugin
   ```

5. Install it by copying the bundle into your OBS plugins folder, then restart OBS:

   ```bash
   mkdir -p "$HOME/Library/Application Support/obs-studio/plugins"
   cp -R build_macos/RelWithDebInfo/draw2-plugin.plugin \
     "$HOME/Library/Application Support/obs-studio/plugins/"
   ```

   Once OBS restarts, the plugin appears as `Draw 2` in the `Docks` menu.

#### Setting up the Python backend

The plugin runs the `draw` backend as a **separate Python process**, it does not
embed an interpreter. You provide a Python installation that has the `draw`
package, and the plugin launches `<prefix>/bin/python` and communicates with it
over shared memory.

> The Python you point to just needs `bin/python` and the `draw` package
> importable. **Any reasonably recent Python 3 works, it does not have to match
> the plugin's build**. A self-contained CPython is the easiest to set up if you don't have
> a python installation already

- You can refer to this repo to get a self-contained CPython [python-build-standalone](https://github.com/astral-sh/python-build-standalone)
- To install the `draw` backend use the **`obs-plugin` branch**, that is the one that exposes the entry point
  the plugin launches (the default `main` branch is the standalone CLI):
   ```shell
   ~/.draw2-runtime/python/bin/python -m pip install "git+https://github.com/HichTala/draw2@obs-plugin"
   ```
- In the Draw 2 settings, set **Select Python installation** to the prefix folder
   (the one that contains `bin/` and `lib/`), e.g. `~/.draw2-runtime/python`. Expected layout:

   ```text
   <prefix>/bin/python
   <prefix>/lib/python3.13/site-packages/draw
   ```

</details>

---

## <div align="center">🔍Method Overview</div>

A medium blog post explaining the main process from data collection to final prediction has been written. 
You can access it at [this](https://medium.com/@hich.tala.phd/how-i-trained-again-my-model-to-detect-and-recognise-a-wide-range-of-yu-gi-oh-cards-5c567a320b0a) address. If you have any questions, don't hesitate to open an issue.

[![Medium](https://img.shields.io/badge/-Medium-12100E?style=flat&logo=medium&labelColor=555)](https://medium.com/@hich.tala.phd/how-i-trained-again-my-model-to-detect-and-recognise-a-wide-range-of-yu-gi-oh-cards-5c567a320b0a)

---

## <div align="center">💬Contact</div>

You can reach me on Twitter [@hichtala](https://twitter.com/hichtala) or by email
at [hich.tala.phd@gmail.com](mailto:hich.tala.phd@gmail.com).

---

## <div align="center">⭐Star History</div>

<div align="center">
<a href="https://www.star-history.com/?repos=HichTala%2Fdraw2&type=date&legend=top-left">
 <picture>
   <source media="(prefers-color-scheme: dark)" srcset="https://api.star-history.com/chart?repos=HichTala/draw2&type=date&theme=dark&legend=top-left&sealed_token=Tj24ueb9rFzr_2PF0RNMdSlbPvsVOwdhhTBjutgQ1SUYji8RrcmHiv_ocH9Awcj-YtsbRvsN3HQ3CXrdvIlH9__zptdgBmaVrYYtCTtq8NSwB3pc8qUZcGcM0Ez6xCI8WWYDgM0H9ZiRDUrW24Rmut3k2YSh3Oe003GY7SfNSXCDbUniUCvwNbTd5ejR" />
   <source media="(prefers-color-scheme: light)" srcset="https://api.star-history.com/chart?repos=HichTala/draw2&type=date&legend=top-left&sealed_token=Tj24ueb9rFzr_2PF0RNMdSlbPvsVOwdhhTBjutgQ1SUYji8RrcmHiv_ocH9Awcj-YtsbRvsN3HQ3CXrdvIlH9__zptdgBmaVrYYtCTtq8NSwB3pc8qUZcGcM0Ez6xCI8WWYDgM0H9ZiRDUrW24Rmut3k2YSh3Oe003GY7SfNSXCDbUniUCvwNbTd5ejR" />
   <img alt="Star History Chart" src="https://api.star-history.com/chart?repos=HichTala/draw2&type=date&legend=top-left&sealed_token=Tj24ueb9rFzr_2PF0RNMdSlbPvsVOwdhhTBjutgQ1SUYji8RrcmHiv_ocH9Awcj-YtsbRvsN3HQ3CXrdvIlH9__zptdgBmaVrYYtCTtq8NSwB3pc8qUZcGcM0Ez6xCI8WWYDgM0H9ZiRDUrW24Rmut3k2YSh3Oe003GY7SfNSXCDbUniUCvwNbTd5ejR" />
 </picture>
</a>
</div>
