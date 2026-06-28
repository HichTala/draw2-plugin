<div align="center">
    <p>
        <img src="https://raw.githubusercontent.com/HichTala/draw2/refs/heads/main/docs/assets/banner.png" alt="DRAW Banner">
    </p>


<div>

[![Web Interface](https://img.shields.io/badge/🌐_Web_Interface-Try_It_Now-blue)](https://hichtala.github.io/draw2)

[![DRAW2 Workflow](https://github.com/HichTala/draw2-plugin/actions/workflows/push.yaml/badge.svg)](https://github.com/HichTala/draw2-plugin/actions/workflows/push.yaml)
[![Licence](https://img.shields.io/pypi/l/ultralytics)](../LICENSE)
[![Github](https://img.shields.io/badge/-github-181717?logo=github&labelColor=555)](https://github.com/HichTala/draw2)
[![Twitter](https://img.shields.io/badge/-twitter-000?logo=x&labelColor=555)](https://twitter.com/hichtala)
[![HuggingFace Downloads](https://img.shields.io/badge/dynamic/json?url=https%3A%2F%2Fhuggingface.co%2Fapi%2Fmodels%2FHichTala%2Fdraw2&query=%24.downloads&logo=huggingface&label=downloads&color=%23FFD21E)](https://huggingface.co/HichTala/draw2)
[![Medium](https://img.shields.io/badge/-Medium-12100E?style=flat&logo=medium&labelColor=555)](https://medium.com/@hich.tala.phd/how-i-trained-again-my-model-to-detect-and-recognise-a-wide-range-of-yu-gi-oh-cards-5c567a320b0a)
[![WandB](https://img.shields.io/badge/visualize_in-W%26B-yellow?logo=weightsandbiases&color=%23FFBE00)](https://wandb.ai/hich_/draw)

[🇬🇧 English](../README.md) | [🇫🇷 Français](README_fr.md) | [🇯🇵 日本語](README_jp.md) | [🇪🇸 Español](README_es.md)


</div>

</div>

DRAW 2 (que significa **D**etect and **R**ecognize **A** **W**ide range of cards version 2 - **D**etectar e **R**econhecer uma **A**mpla gama de cartas versão "**W**" 2) é um detector de objetos treinado para detectar cartas de _Yu-Gi-Oh!_ em todos os tipos de imagens e em particular, imagens de duelos.

Esse projeto é a parte do plugin do sistema DRAW 2. Ele permite usuários **sem nenhuma experiência técnica** a integrar perfeitamente o detector diretamente nas suas live streams ou vídeos gravados.
O plugin pode exibir cartas detectadas em tempo real para uma experiência de visualização elevada.
O projeto de backend está disponível [aqui](https://github.com/HichTala/draw2).

Esse projeto é licenciado sob [GNU Affero General Public License v3.0](LICENCE); qualquer contribuição é bem-vinda.

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

## <div align="center">Documentação</div>

### 🛠️ Instalação

Siga a instrução de instalação dependendo do seu sistema operacional para que tudo funcione perfeitamente:

<details open>
<summary>🪟 Windows</summary>

1. Baixe o instalador do plugin deste
   link: [DRAW2 Plugin Installer](https://github.com/HichTala/draw2-plugin/releases/download/0.2.1/draw2-plugin-installer.exe)
2. Execute o instalador e siga as instruções na tela.
3. Assim que a instalação estiver concluída, execute o OBS Studio. Se tudo foi configurado corretamente,
   deve-se ver no menu `Painéis` uma nova opção chamada `Draw 2`.
   Você pode ativar o painel e colocá-lo onde quiser.

   O download foi concluído!
</details>

<details>
<summary>🐧 Linux</summary>

1. Baixe o instalador do plugin deste link:
   [DRAW2 Plugin Installer](https://github.com/HichTala/draw2-plugin/releases/download/0.2.1/draw2-plugin-0.2.1-x86_64-linux-gnu.deb)

2. Execute o instalador clicando duas vezes nele e depois em instalar _OU_ executando o comando:

   ```shell
   sudo apt install ./draw2-plugin-0.2.1-x86_64-linux-gnu.deb
   ```

3. Assim que a instalação estiver concluída, execute o OBS Studio. Se tudo foi configurado corretamente, deve-se ver
   no menu `Painéis` uma nova opção chamada `Draw 2`. Você pode ativar o painel e colocá-lo onde quiser. A instalação
   ainda não está completa: o plugin está instalado, mas você ainda precisa instalar o backend Python. Feche o OBS e
   siga os próximos passos.

4. Se você já tem uma instalação do Python que quer usar, pode pular este passo. Qualquer instalação do Python com o
   pacote `draw` instalado pode funcionar. Vamos usar um CPython autônomo. Uma build relocável do
   [python-build-standalone](https://github.com/astral-sh/python-build-standalone):

   ```shell
   # escolha a build install_only para a sua arquitetura (x86_64 para CPUs Intel e AMD)
   curl -fL -o python.tar.gz https://github.com/astral-sh/python-build-standalone/releases/download/20261009/cpython-3.13.16+20261009-x86_64-unknown-linux-gnu-install_only.tar.gz
   mkdir -p ~/.draw2-runtime && tar -xzf python.tar.gz -C ~/.draw2-runtime
   ```

5. Instale o backend `draw` (certifique-se de que o [git](https://git-scm.com/install/linux) está instalado):

   ```shell
   ~/.draw2-runtime/python/bin/python -m pip install "git+https://github.com/HichTala/draw2@obs-plugin"
   ```

6. Nos ajustes do Draw 2, defina **Select Python installation** como a pasta do prefixo
   (a que contém `bin/` e `lib/`), p. ex. `~/.draw2-runtime/python`.
   Estrutura esperada:

   ```text
   <prefix>/bin/python
   <prefix>/lib/python3.13/site-packages/draw
   ```

A instalação foi concluída, aproveite a detecção!
</details>

<details>
<summary>🍏 MacOS</summary>

1. Baixe o instalador do plugin deste link:
   [DRAW2 Plugin Installer]() (a build para macOS ainda não foi lançada, estamos trabalhando nisso e ela será lançada
   em breve; enquanto isso, você ainda pode usar o plugin compilando-o a partir do código-fonte, veja a seção
   [Building from source](../README.md) do README em inglês)

2. Execute o instalador clicando duas vezes nele. Você verá um aviso dizendo que a Apple não conseguiu verificar o
   plugin; feche-o (`Done`), vá para os ajustes do sistema e procure por `Privacidade e Segurança`
   (`Privacy & Security`). Role para baixo até ver `"draw2-plugin....pkg" was blocked to protect your Mac` (a mensagem
   pode aparecer no seu idioma), clique em `Abrir Mesmo Assim` (`Open Anyway`), clique em `Abrir Mesmo Assim` de novo e
   siga as instruções na tela.

3. Assim que a instalação estiver concluída, execute o OBS Studio. Se tudo foi configurado corretamente, deve-se ver
   no menu `Painéis` uma nova opção chamada `Draw 2`. Você pode ativar o painel e colocá-lo onde quiser. A instalação
   ainda não está completa: o plugin está instalado, mas você ainda precisa instalar o backend Python. Feche o OBS e
   siga os próximos passos.

4. Se você já tem uma instalação do Python que quer usar, pode pular este passo. Qualquer instalação do Python com o
   pacote `draw` instalado pode funcionar. Vamos usar um CPython autônomo. Uma build relocável do
   [python-build-standalone](https://github.com/astral-sh/python-build-standalone):

   ```shell
   # escolha a build install_only para a sua arquitetura (aarch64 para Apple Silicon)
   curl -fL -o python.tar.gz https://github.com/astral-sh/python-build-standalone/releases/download/20261009/cpython-3.13.16+20261009-aarch64-apple-darwin-install_only.tar.gz
   mkdir -p ~/.draw2-runtime && tar -xzf python.tar.gz -C ~/.draw2-runtime
   ```

5. Instale o backend `draw` (certifique-se de que o [git](https://git-scm.com/install/mac) está instalado):

   ```shell
   ~/.draw2-runtime/python/bin/python -m pip install "git+https://github.com/HichTala/draw2@obs-plugin"
   ```

6. Nos ajustes do Draw 2, defina **Select Python installation** como a pasta do prefixo
   (a que contém `bin/` e `lib/`), p. ex. `~/.draw2-runtime/python`.
   Estrutura esperada:

   ```text
   <prefix>/bin/python
   <prefix>/lib/python3.13/site-packages/draw
   ```

A instalação foi concluída, aproveite a detecção!
</details>

### 🚀 Uso

Quando o plugin está instalado e os "model weights" estão baixados, você pode executar o OBS Studio.

1. Abra o menu `Painéis` e selecione `Draw 2` para ativar o painel do plugin.
2. No painel do Draw 2, você pode configurar os ajustes clicando no ícone de engrenagem ao lado do botão `Start DRAW`:
   - **Select Python installation**: Caminho para o prefixo do Python que tem o backend `draw` instalado (a pasta que
     contém `bin/` e `lib/`). Deve ser uma instalação completa do Python, não um virtualenv.
   - **Select Deck Lists**: Escolha os arquivos de decklist que contenham as cartas que você quer detectar — até 3. Para adicionar decklists, clique em `Open Folder` e arraste seus arquivos `.ydk` para a pasta que foi aberta.
   - **Minimum Out of Screen Time**: O tempo mínimo que uma carta recém detectada pode ser exibida de novo.
   - **Minimum Screen Time**: O tempo mínimo que uma carta é exibida.
   - **Confidence Threshold**: Definir o nível de confiança mínima para a detecção de uma carta. Detecções abaixo desse limite
     serão ignoradas.
   - **Advanced features** (desativadas por padrão): dois ajustes opcionais da entrada do detector que afetam
     apenas o que o detector vê, sem impacto na sua saída ao vivo. Ative aqui e configure os valores na fonte
     `Draw Display`:
     - **Enable two players** — adiciona um conjunto de decklists do **Player 2** e executa um **detector separado por jogador**, permitindo processar dois jogadores ao mesmo tempo. Deixe desativado para uma configuração normal de um único jogador.
     - **Enable detector input crop** — adiciona os campos **Crop (Left/Top/Right/Bottom, px)** à fonte, para
       focar a detecção na região onde as cartas são colocadas.
     - **Enable 180° input rotation** — adiciona o botão **Rotate input 180°** à fonte, para câmera montada de
       cabeça para baixo.
     - Quando qualquer uma das duas está ativada, a fonte também ganha o botão **Preview detector input**:
       ative-o para que a fonte exiba o quadro recortado/rotacionado que ela envia ao detector (para ajustar
       o recorte direto no preview da fonte) e desative-o para voltar a exibir as cartas detectadas.
   - **Enable debug logging** (desativado por padrão): exibe diagnósticos detalhados — o log do painel mostra
     o lançamento exato do backend (caminhos de deck resolvidos, canal, limites) e cada linha do backend sem
     filtro, e o log do OBS recebe os detalhes de captura (fonte de entrada, dimensões, recorte). Útil
     quando a detecção não está se comportando como esperado.
3. O plugin irá fornecer uma nova fonte chamada `Draw Display`. Você pode adicioná-la a sua cena como qualquer outra fonte.
   Essa fonte irá exibir as cartas detectadas na tela. Você pode escolher de qual fonte/cena detectar as cartas.
   Com o modo de dois jogadores ativado, use a propriedade **Detector / Player** da fonte para escolher de qual detector ela lê (**Player 1** ou **Player 2**); adicione um `Draw Display` por jogador para exibir ambos ao mesmo tempo.
4. Clique no botão `Start DRAW` para começar o processo de detecção. O plugin irá começar a detectar cartas em tempo real
   e exibí-las na tela usando a fonte `Draw Display`. O plugin irá começar a detectar a partir do momento que você vir o botão `Stop DRAW`.
   Se não aparecer, algo deu errado.
5. Dando tudo certo, aproveite o plugin!

Aqui está uma pequena prévia :)
<div align="center">
    <img src="https://raw.githubusercontent.com/HichTala/draw2/refs/heads/main/docs/assets/overview.gif" width="960" height="540" />
</div>

### ⚙️ Compilando a partir do código-fonte

> As instruções para compilar o plugin a partir do código-fonte (Windows, Linux e macOS), incluindo a configuração
> do backend Python, estão disponíveis apenas em inglês. Consulte a seção **Building from source** do
> [README em inglês](../README.md).

---
## <div align="center">🔍Visão Geral do Método</div>

Um post de blog no Medium explicando o processo principal da coleta de dados até a previsão final foi escrita.
Você pode acessar ele [neste](https://medium.com/@hich.tala.phd/how-i-trained-again-my-model-to-detect-and-recognise-a-wide-range-of-yu-gi-oh-cards-5c567a320b0a) endereço. Se você tiver quaisquer perguntas, não hesite em abrir uma issue.

[![Medium](https://img.shields.io/badge/-Medium-12100E?style=flat&logo=medium&labelColor=555)](https://medium.com/@hich.tala.phd/how-i-trained-again-my-model-to-detect-and-recognise-a-wide-range-of-yu-gi-oh-cards-5c567a320b0a)

---

## <div align="center">Contato</div>

Você pode me contatar pelo X(Twitter) [@hichtala](https://twitter.com/hichtala) ou por e-mail [hich.tala.phd@gmail.com](mailto:hich.tala.phd@gmail.com).

---

## <div align="center">⭐Histórico de Estrelas</div>

<div align="center">
<a href="https://www.star-history.com/#hichtala/draw2&type=date&legend=top-left">
 <picture>
   <source media="(prefers-color-scheme: dark)" srcset="https://api.star-history.com/svg?repos=hichtala/draw2&type=date&theme=dark&legend=top-left" />
   <source media="(prefers-color-scheme: light)" srcset="https://api.star-history.com/svg?repos=hichtala/draw2&type=date&legend=top-left" />
   <img alt="Star History Chart" src="https://api.star-history.com/svg?repos=hichtala/draw2&type=date&legend=top-left" />
 </picture>
</a>
</div>