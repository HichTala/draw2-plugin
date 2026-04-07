<div align="center">
    <p>
        <img src="https://raw.githubusercontent.com/HichTala/draw2/refs/heads/main/figures/banner-draw.png" alt="DRAW Banner">
    </p>

[![DRAW2 Workflow](https://github.com/HichTala/draw2-plugin/actions/workflows/push.yaml/badge.svg)](https://github.com/HichTala/draw2-plugin/actions/workflows/push.yaml)
[![Licence](https://img.shields.io/pypi/l/ultralytics)](../LICENSE)
[![Github](https://img.shields.io/badge/-github-181717?logo=github&labelColor=555)](https://github.com/HichTala/draw2)
[![Twitter](https://img.shields.io/badge/-twitter-000?logo=x&labelColor=555)](https://twitter.com/hichtala)
[![HuggingFace Downloads](https://img.shields.io/badge/dynamic/json?url=https%3A%2F%2Fhuggingface.co%2Fapi%2Fmodels%2FHichTala%2Fdraw2&query=%24.downloads&logo=huggingface&label=downloads&color=%23FFD21E)](https://huggingface.co/HichTala/draw2)
[![Medium](https://img.shields.io/badge/-Medium-12100E?style=flat&logo=medium&labelColor=555)](https://medium.com/@hich.tala.phd/how-i-trained-again-my-model-to-detect-and-recognise-a-wide-range-of-yu-gi-oh-cards-5c567a320b0a)
[![WandB](https://img.shields.io/badge/visualize_in-W%26B-yellow?logo=weightsandbiases&color=%23FFBE00)](https://wandb.ai/hich_/draw)

[🇬🇧 English](../README.md) | [🇫🇷 Français](README_fr.md) | [🇧🇷 Português](README_pt-br.md)

</div>

DRAW 2（**D**etect and **R**ecognize **A** **W**ide range of cards version 2の略）は、あらゆる画像、特にデュエル中の画像から遊戯王カードを検出するために開発されたAIです。

このプロジェクトはDRAW 2システムのプラグイン部分です。**プログラミングの知識がなくても**、ライブ配信や録画動画に検出機能を簡単に統合できます。プラグインはリアルタイムで検出されたカードを表示し、視聴体験を向上させます。
Pythonバックエンドのプロジェクトは[こちら](https://github.com/HichTala/draw2)で公開されています。

このプロジェクトは[GNU Affero General Public License v3.0](https://github.com/HichTala/draw2-plugin/blob/master/LICENCE)のもとで公開されており、どなたでも参加できます！
---
## <div align="center">📰 ニュース</div>

> 🃏 **対応している最新の拡張機能:** `MZMU` --- 最終更新日 `06-04-2026`  
> 🔧 **最新版:** `0.2.0-beta` --- 最終更新日 `06-04-2026`

---

## <div align="center">📄 ドキュメント</div>

### 🛠️ インストール

OSに応じた手順に従ってセットアップしてください。

<details open>
<summary>🪟 Windows</summary>

1. プラグインのインストーラーを[こちら](https://github.com/HichTala/draw2-plugin/releases/download/0.2.0/draw2-plugin-installer.exe)からダウンロードします。
2. インストーラーを実行し、画面の指示に従ってください。
3. インストールが完了したらOBS Studioを起動します。`Docks` メニューに `Draw 2` が表示されるので、有効化して好きな位置に配置します。
4. まだインストールは完了していません。DRAW 2のモデルウェイトをダウンロードする必要があります。OBS Studioを閉じ、プラグインがインストールされたフォルダを開きます（デフォルト:`C:\Program Files\draw2`）。`python` フォルダを開き、右クリックして「ターミナルで開く」を選択します。
5. ターミナルで以下のコマンドを実行し、モデルウェイトをダウンロードします。

   ```
   ./python.exe -c "import draw;draw.run()"
   ```

6. 以下のメッセージが表示されたらダウンロード完了です。OBS Studioを再起動してください。

   ```
   Running Draw2 without OBS shared memory
   Waiting for OBS to start...
   ```
</details>
<details>
<summary>🐧 Linux</summary>

準備中です 👀

</details>

<details>
<summary>🍏 MacOS</summary>

MacOSでのOBSプラグインに詳しくないため、信頼性のあるインストールガイドを提供できません。MacOSでのコンパイルは成功していますが、十分なテストを行っていません。MacOSでのOBSプラグインに詳しい方がいれば、インストールガイドの作成にぜひ協力ください。Pull Requestをお待ちしています。
</details>

### 🚀 使い方

プラグインのインストールとモデルウェイトのダウンロードが完了したら、OBS Studioを起動します。

1. `Docks` メニューから `Draw 2` を選択し、プラグインドックを有効化します。
2. Draw 2ドック内で、`Start DRAW` ボタン横の歯車アイコンをクリックして設定を行います。
   * **デッキリストの選択**:検出したいカードが含まれるデッキリストファイルを選択します。同時に3つのデッキリストを扱えます。新しいデッキリストを追加するには、`Open Folder` ボタンをクリックし、ydk形式のファイルをフォルダにドラッグアンドドロップします。
   * **画面外表示の最小時間**:検出されたカードが再表示されるまでの最小時間を設定します。
   * **画面表示の最小時間**:カードが表示される最小時間を設定します。
   * **信頼度の閾値**:カード検出の最小信頼度を設定します。この閾値以下の検出は無視されます。
3. プラグインは `Draw Display` という新しいソースを提供します。シーンに追加すると、検出されたカードが画面上に表示されます。どのソースやシーンからカードを検出するか選択できます。
4. `Start DRAW` ボタンをクリックして検出を開始します。プラグインはリアルタイムでカードを検出し、`Draw Display` ソースを使って画面に表示します。
5. プラグインをお楽しみください！

ちょっとだけご紹介します :)
<div align="center">
    <img src="https://raw.githubusercontent.com/HichTala/draw2/refs/heads/main/figures/overview.gif" width="960" height="540" />
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
