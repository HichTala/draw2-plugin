<div align="center">
    <p>
        <img src="https://raw.githubusercontent.com/HichTala/draw2/refs/heads/main/docs/assets/banner.png" alt="DRAW Banner">
    </p>

[![Web Interface](https://img.shields.io/badge/🌐_Web_Interface-Try_It_Now-blue)](https://hichtala.github.io/draw2)

[![DRAW2 Workflow](https://github.com/HichTala/draw2-plugin/actions/workflows/push.yaml/badge.svg)](https://github.com/HichTala/draw2-plugin/actions/workflows/push.yaml)
[![Licence](https://img.shields.io/pypi/l/ultralytics)](../LICENSE)
[![Github](https://img.shields.io/badge/-github-181717?logo=github&labelColor=555)](https://github.com/HichTala/draw2)
[![Twitter](https://img.shields.io/badge/-twitter-000?logo=x&labelColor=555)](https://twitter.com/hichtala)
[![HuggingFace Downloads](https://img.shields.io/badge/dynamic/json?url=https%3A%2F%2Fhuggingface.co%2Fapi%2Fmodels%2FHichTala%2Fdraw2&query=%24.downloads&logo=huggingface&label=downloads&color=%23FFD21E)](https://huggingface.co/HichTala/draw2)
[![Medium](https://img.shields.io/badge/-Medium-12100E?style=flat&logo=medium&labelColor=555)](https://medium.com/@hich.tala.phd/how-i-trained-again-my-model-to-detect-and-recognise-a-wide-range-of-yu-gi-oh-cards-5c567a320b0a)
[![WandB](https://img.shields.io/badge/visualize_in-W%26B-yellow?logo=weightsandbiases&color=%23FFBE00)](https://wandb.ai/hich_/draw)

[🇬🇧 English](../README.md) | [🇫🇷 Français](README_fr.md) | [🇧🇷 Português](README_pt-br.md) | [🇪🇸 Español](README_es.md)

</div>

DRAW 2（**D**etect and **R**ecognize **A** **W**ide range of cards version 2の略）は、あらゆる画像、特にデュエル中の画像から遊戯王カードを検出するために開発されたAIです。

このプロジェクトはDRAW 2システムのプラグイン部分です。**プログラミングの知識がなくても**、ライブ配信や録画動画に検出機能を簡単に統合できます。プラグインはリアルタイムで検出されたカードを表示し、視聴体験を向上させます。
Pythonバックエンドのプロジェクトは[こちら](https://github.com/HichTala/draw2)で公開されています。

このプロジェクトは[GNU Affero General Public License v3.0](https://github.com/HichTala/draw2-plugin/blob/master/LICENCE)のもとで公開されており、どなたでも参加できます！

---

## <div align="center">📰 ニュース</div>

> 🃏 **対応している最新の拡張機能:** `CORI` --- 最終更新日 `13-06-2026`  
> 🔧 **最新版:** `0.2.1-beta` --- 最終更新日 `01-06-2026`

---

## <div align="center">📄 ドキュメント</div>

### 🛠️ インストール

OSに応じた手順に従ってセットアップしてください。

<details open>
<summary>🪟 Windows</summary>

1. プラグインのインストーラーを[こちら](https://github.com/HichTala/draw2-plugin/releases/download/0.2.1/draw2-plugin-installer.exe)からダウンロードします。
2. インストーラーを実行し、画面の指示に従ってください。
3. インストールが完了したらOBS Studioを起動します。`Docks` メニューに `Draw 2` が表示されるので、有効化して好きな位置に配置します。

インストールは完了です。検出をお楽しみください！

</details>

<details>
<summary>🐧 Linux</summary>

1. プラグインのインストーラーを[こちら](https://github.com/HichTala/draw2-plugin/releases/download/0.2.1/draw2-plugin-0.2.1-x86_64-linux-gnu.deb)からダウンロードします。

2. インストーラーをダブルクリックして「インストール」をクリックするか、次のコマンドを実行します。

   ```shell
   sudo apt install ./draw2-plugin-0.2.1-x86_64-linux-gnu.deb
   ```

3. インストールが完了したらOBS Studioを起動します。`Docks` メニューに `Draw 2` が表示されます。ただし、インストールはまだ完了していません。プラグインはインストールされましたが、Pythonバックエンドのインストールが必要です。OBS Studioを閉じて、次の手順に進んでください。

4. 使いたいPythonがすでにある場合は、この手順は不要です。`draw2` パッケージがインストールされたPythonであれば動作します。ここでは、自己完結型のCPythonを使用します（[python-build-standalone](https://github.com/astral-sh/python-build-standalone)）。

   ```shell
   # お使いのアーキテクチャに合った install_only ビルドを選んでください（IntelおよびAMDのCPUの場合は x86_64）
   curl -fL -o python.tar.gz https://github.com/astral-sh/python-build-standalone/releases/download/20261009/cpython-3.13.16+20261009-x86_64-unknown-linux-gnu-install_only.tar.gz
   mkdir -p ~/.draw2-runtime && tar -xzf python.tar.gz -C ~/.draw2-runtime
   ```

5. `draw` バックエンドをインストールします（[git](https://git-scm.com/install/linux)がインストールされていることを確認してください）。

   ```shell
   ~/.draw2-runtime/python/bin/python -m pip install "git+https://github.com/HichTala/draw2@obs-plugin"
   ```

6. Draw 2の設定で、**Pythonインストールの選択** にプレフィックスフォルダ（`bin/` と `lib/` を含むフォルダ）を指定します。例：`~/.draw2-runtime/python`
   期待されるフォルダ構成：

   ```text
   <prefix>/bin/python
   <prefix>/lib/python3.13/site-packages/draw
   ```

インストールは完了です。検出をお楽しみください！

</details>

<details>
<summary>🍏 MacOS</summary>

1. プラグインのインストーラーを[こちら]()からダウンロードします（macOS版はまだリリースされていません。現在開発中で、まもなく公開予定です。それまではソースからビルドしてご利用いただけます。[英語版README](../README.md)の **Building from source** セクションを参照してください）。

2. インストーラーをダブルクリックして実行します。Appleがプラグインを検証できなかった旨のポップアップが表示されるので閉じます（`Done`）。次にシステム設定を開き、`プライバシーとセキュリティ`（`Privacy & Security`）を検索します。下にスクロールして `"draw2-plugin....pkg" was blocked to protect your Mac`（お使いの言語で表示される場合があります）を見つけ、`このまま開く`（`Open Anyway`）を2回クリックし、画面の指示に従ってください。

3. インストールが完了したらOBS Studioを起動します。`Docks` メニューに `Draw 2` が表示されます。ただし、インストールはまだ完了していません。プラグインはインストールされましたが、Pythonバックエンドのインストールが必要です。OBS Studioを閉じて、次の手順に進んでください。

4. 使いたいPythonがすでにある場合は、この手順は不要です。`draw2` パッケージがインストールされたPythonであれば動作します。ここでは、自己完結型のCPythonを使用します（[python-build-standalone](https://github.com/astral-sh/python-build-standalone)）。

   ```shell
   # お使いのアーキテクチャに合った install_only ビルドを選んでください（Apple Siliconの場合は aarch64）
   curl -fL -o python.tar.gz https://github.com/astral-sh/python-build-standalone/releases/download/20261009/cpython-3.13.16+20261009-aarch64-apple-darwin-install_only.tar.gz
   mkdir -p ~/.draw2-runtime && tar -xzf python.tar.gz -C ~/.draw2-runtime
   ```

5. `draw` バックエンドをインストールします（[git](https://git-scm.com/install/mac)がインストールされていることを確認してください）。

   ```shell
   ~/.draw2-runtime/python/bin/python -m pip install "git+https://github.com/HichTala/draw2@obs-plugin"
   ```

6. Draw 2の設定で、**Pythonインストールの選択** にプレフィックスフォルダ（`bin/` と `lib/` を含むフォルダ）を指定します。例：`~/.draw2-runtime/python`
   期待されるフォルダ構成：

   ```text
   <prefix>/bin/python
   <prefix>/lib/python3.13/site-packages/draw
   ```

インストールは完了です。検出をお楽しみください！

</details>

### 🚀 使い方

プラグインのインストールとモデルウェイトのダウンロードが完了したら、OBS Studioを起動します。

1. `Docks` メニューから `Draw 2` を選択し、プラグインドックを有効化します。
2. Draw 2ドック内で、`Start DRAW` ボタン横の歯車アイコンをクリックして設定を行います。
   * **Pythonインストールの選択**:`draw` バックエンドがインストールされたPythonプレフィックス（`bin/` と `lib/` を含むフォルダ）のパスを指定します。仮想環境（virtualenv）ではなく、完全なPythonインストールである必要があります。詳細は上記のLinux/macOSのインストール手順を参照してください。
   * **デッキリストの選択**:検出したいカードが含まれるデッキリストファイルを選択します。同時に3つのデッキリストを扱えます。新しいデッキリストを追加するには、`Open Folder` ボタンをクリックし、ydk形式のファイルをフォルダにドラッグアンドドロップします。
   * **画面外表示の最小時間**:検出されたカードが再表示されるまでの最小時間を設定します。
   * **画面表示の最小時間**:カードが表示される最小時間を設定します。
   * **信頼度の閾値**:カード検出の最小信頼度を設定します。この閾値以下の検出は無視されます。
   * **Advanced features**（デフォルトでオフ）：ライブ出力には影響せず、検出器が見る映像のみに作用する2つのオプション設定です。ここで有効にし、`Draw Display` ソースで値を設定してください：
     * **Enable detector input crop** — カードが置かれる領域に検出を絞り込むため、ソースに **Crop (Left/Top/Right/Bottom, px)** フィールドを追加します。
     * **Enable 180° input rotation** — カメラが逆さまに取り付けられている場合のために、ソースに **Rotate input 180°** トグルを追加します。
     * どちらかが有効になると、ソースに **Preview detector input** トグルも追加されます：オンにするとソースが検出器に送るクロップ／回転済みのフレームを表示し（ソースのプレビューで直接クロップを調整できます）、オフにすると検出されたカードの表示に戻ります。
3. プラグインは `Draw Display` という新しいソースを提供します。シーンに追加すると、検出されたカードが画面上に表示されます。どのソースやシーンからカードを検出するか選択できます。
4. `Start DRAW` ボタンをクリックして検出を開始します。プラグインはリアルタイムでカードを検出し、`Draw Display` ソースを使って画面に表示します。`Stop DRAW` ボタンが表示された時点から検出が始まります。表示されない場合は、何か問題が発生しています。
5. 問題がなければ、プラグインをお楽しみください！

ちょっとだけご紹介します :)
<div align="center">
    <img src="https://raw.githubusercontent.com/HichTala/draw2/refs/heads/main/docs/assets/overview.gif" width="960" height="540" />
</div>

---

## <div align="center">🔍 技術的な詳細</div>

データ収集から実際の認識までのプロセスについては、[Mediumの記事](https://medium.com/@hich.tala.phd/how-i-trained-again-my-model-to-detect-and-recognise-a-wide-range-of-yu-gi-oh-cards-5c567a320b0a)で詳しく解説しています。質問があればお気軽にIssueを立ててください。

---

##  <div align="center">💬 連絡先</div>

- Twitter: [@hichtala](https://twitter.com/hichtala)
- メール: [hich.tala.phd@gmail.com](mailto:hich.tala.phd@gmail.com)

質問やアイデアがあれば、IssueまたはSNSまでお気軽にご連絡ください！

---

## <div align="center">⭐Star History</div>

<a href="https://www.star-history.com/#HichTala/draw2&type=date&legend=top-left">
 <picture>
   <source media="(prefers-color-scheme: dark)" srcset="https://api.star-history.com/svg?repos=HichTala/draw2&type=date&theme=dark&legend=top-left" />
   <source media="(prefers-color-scheme: light)" srcset="https://api.star-history.com/svg?repos=HichTala/draw2&type=date&legend=top-left" />
   <img alt="Star History Chart" src="https://api.star-history.com/svg?repos=HichTala/draw2&type=date&legend=top-left" />
 </picture>
</a>