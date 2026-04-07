<div align="center">
    <p>
        <img src="https://raw.githubusercontent.com/HichTala/draw2/refs/heads/main/figures/banner-draw.png" alt="DRAW Banner">
    </p>


<div>

[![DRAW2 Workflow](https://github.com/HichTala/draw2-plugin/actions/workflows/push.yaml/badge.svg)](https://github.com/HichTala/draw2-plugin/actions/workflows/push.yaml)
[![Licence](https://img.shields.io/pypi/l/ultralytics)](LICENSE)
[![Github](https://img.shields.io/badge/-github-181717?logo=github&labelColor=555)](https://github.com/HichTala/draw2)
[![Twitter](https://img.shields.io/badge/-twitter-000?logo=x&labelColor=555)](https://twitter.com/hichtala)
[![HuggingFace Downloads](https://img.shields.io/badge/dynamic/json?url=https%3A%2F%2Fhuggingface.co%2Fapi%2Fmodels%2FHichTala%2Fdraw2&query=%24.downloads&logo=huggingface&label=downloads&color=%23FFD21E)](https://huggingface.co/HichTala/draw2)
[![Medium](https://img.shields.io/badge/-Medium-12100E?style=flat&logo=medium&labelColor=555)](https://medium.com/@hich.tala.phd/how-i-trained-again-my-model-to-detect-and-recognise-a-wide-range-of-yu-gi-oh-cards-5c567a320b0a)
[![WandB](https://img.shields.io/badge/visualize_in-W%26B-yellow?logo=weightsandbiases&color=%23FFBE00)](https://wandb.ai/hich_/draw)

[🇫🇷 Français](readmes/README_fr.md) | [🇧🇷 Português](readmes/README_pt-br.md) | [🇯🇵 日本人語](readmes/README_jp.md)


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

> 🃏 **Latest card pool:** `MZMU` --- last updated `06-04-20265`  
> 🔧 **Latest app version:** `0.2.0-beta` --- last updated `06-04-2026`

<table>
  <tr>
    <th>Date</th>
    <th>Type</th>
    <th>Description</th>
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
   link: [DRAW2 Plugin Installer](https://github.com/HichTala/draw2-plugin/releases/download/0.2.0/draw2-plugin-installer.exe)
2. Run the installer and follow the on-screen instructions.
3. Once the installation is complete, launch OBS Studio. If everything is set up correctly, you should see in the
   `Docks` menu
   a new option called `Draw 2`. You can activate the dock and dock it wherever you want.
   
   The download is complete, you can enjoy detecting !

</details>

<details>
<summary>🐧 Linux</summary>

Coming soon 👀
</details>

<details>
<summary>🍏 MacOS</summary>

I'm not familiar enough with OBS on macOS to provide a reliable installation guide.
The plugin is able to compile successfully on macOS, but I haven't tested it thoroughly.
If you have experience with OBS plugins on macOS and would like to contribute an installation guide,
please feel free to submit a pull request.
</details>

### 🚀 Usage

When the plugin is installed and the model weights are downloaded, you can launch OBS Studio.

1. Open the `Docks` menu and select `Draw 2` to activate the plugin dock.
2. In the Draw 2 dock, you can configure the plugin settings by clicking on the gear icon next to `Start DRAW` button:
    - **Select Deck List**: Choose the deck list file that contains the cards you want to detect. 3 deck lists
      can be handled at the same time. To add new deck lists, you can click the `Open Folder` button and drag and drop
      your deck list files (in ydk format) into the opened folder.
    - **Minimum Out of Screen Time**: The minimum time a card just detected can be displayed again.
    - **Minimum Screen Time**: The minimum time a card is displayed.
    - **Confidence Threshold**: Set the minimum confidence level for card detection. Detections below this threshold
      will be ignored.
3. The plugin provide a new source called `Draw Display`. You can add it to your scene like any other source.
   This source will display the detected cards on the screen. You can choose what source/scene to detect cards from.
4. Click the `Start DRAW` button to start the detection process. The plugin will start detecting cards in real time
   and display them on the screen using the `Draw Display` source. The plugin start detecting from the moment you see
   the
   `Stop DRAW` button. If you don't see it something went wrong.
5. In the other case you can enjoy the plugin!

Here is a small overview :)
<div align="center">
    <img src="https://raw.githubusercontent.com/HichTala/draw2/refs/heads/main/figures/overview.gif" width="960" height="540" />
</div>

---
## <div align="center">🔍Method Overview</div>

A medium blog post explainng the main process from data collection to final prediction has been written. 
You can access it at [this](https://medium.com/@hich.tala.phd/how-i-trained-again-my-model-to-detect-and-recognise-a-wide-range-of-yu-gi-oh-cards-5c567a320b0a) adress. If you have any questions, don't hesitate to open an issue.

[![Medium](https://img.shields.io/badge/-Medium-12100E?style=flat&logo=medium&labelColor=555)](https://medium.com/@hich.tala.phd/how-i-trained-again-my-model-to-detect-and-recognise-a-wide-range-of-yu-gi-oh-cards-5c567a320b0a)

---

## <div align="center">💬Contact</div>

You can reach me on Twitter [@hichtala](https://twitter.com/hichtala) or by email
at [hich.tala.phd@gmail.com](mailto:hich.tala.phd@gmail.com).

---

## <div align="center">⭐Star History</div>

<div align="center">
<a href="https://www.star-history.com/#hichtala/draw2&type=date&legend=top-left">
 <picture>
   <source media="(prefers-color-scheme: dark)" srcset="https://api.star-history.com/svg?repos=hichtala/draw2&type=date&theme=dark&legend=top-left" />
   <source media="(prefers-color-scheme: light)" srcset="https://api.star-history.com/svg?repos=hichtala/draw2&type=date&legend=top-left" />
   <img alt="Star History Chart" src="https://api.star-history.com/svg?repos=hichtala/draw2&type=date&legend=top-left" />
 </picture>
</a>
</div>
