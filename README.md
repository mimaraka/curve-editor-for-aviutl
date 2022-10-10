# Curve Editor v0.2 for AviUtl
![graph](https://user-images.githubusercontent.com/106879397/194698464-c35760d9-eab2-44f7-8082-c3e4a58a8380.png)  

ベジェ曲線のイージングをウィンドウ上で編集できるAviUtlプラグインです。  

## ダウンロード
**v0.2**(テスト版)：https://github.com/mimaraka/aviutl-plugin-curve_editor/releases/tag/v0.2    
※カーブを編集するUIと、カーブの適用・プロジェクトファイルへの保存機能が実装されています。

## インストール
同梱されている`curve_editor.auf`を`aviutl.exe`のあるフォルダもしくは`plugins`フォルダ内に入れ、`@Curve Editor.tra`を`script`フォルダまたはその下のフォルダに入れてください。

## 使用方法
AviUtlを起動後、メインウィンドウの「表示」メニューから「Curve Editorの表示」をクリックすると、Curve EditorのGUIが表示されます。

本プラグインでは、以下で紹介する「Valueモード」と「IDモード」の2つの編集モードでカーブを編集することができます。

- #### Valueモード
    Valueモードでは、カーブを1~10桁の整数で識別します。値とカーブがそれぞれ対応しています。

    ウィンドウ下部の「Copy」ボタンをクリックすると、クリップボードにカーブの値がコピーされます。アニメーションカーブを設定したいパラメータの移動方法に`Value@Curve Editor`を選択し、「移動フレーム間隔」の数値入力欄でコピーした値を貼り付けるとカーブが適用されます。

- #### IDモード
    IDモードでは、0~1023までのグラフのIDを使って、最大1024個のカーブをプロジェクトファイルに保存できます。  
    ベジェ軌道スクリプトと同様に、パラメータにIDを入力することで作成したイージングが使用できます。  
    
    このモードでは、制御点を追加することでマルチベジェ軌道のように複雑なカーブを作成することもできます。  
    グラフ上の何もないところをダブルクリックすると制御点が追加され、制御点上で再度ダブルクリックすると削除されます。  

| | Valueモード | IDモード |
| :---: | :---: | :---: |
| マルチベジェ軌道 | × | 〇|
| カーブの一括変更 | × | 〇|
| exa, exo出力 | 〇 | ×|
| .flowファイル読み込み | 〇 | 〇|
| .flowファイル書き出し | 〇 | ×|
| .cepファイル読み込み | 〇 | 〇|
| .cepファイル書き出し | 〇 | 〇|
| ライブラリから直接D&D | 〇 | ×|
| その他 | 制御点のY座標に制限あり | カーブの個数に制限あり|

右クリックメニューの「編集モード」から2つのモードを切り替えることができます。

### グラフ
[Shift]や[Ctrl]、[Alt]のキーを押しながらドラッグすると次のような操作をすることができます。(Valueモードのみ)  
| [Shift] | [Alt] | [Ctrl] | [Shift]+[Ctrl] |
| :---: | :---: | :---: | :---: |
| 制御点をグラフ上下の辺にスナップ | ハンドルの角度を固定 | ハンドルの長さを固定 | 2つの制御点を同時に動かす |

### 右クリックメニュー
カーブエディタ内で右クリックをすると次のようなオプションを選択できます：

- #### 値をコピー(Valueモードのみ)
    カーブの値をクリップボードにコピーします。Copyボタンと動作は同じです。

- #### 保存(未実装)
    カーブをプリセットとして保存します。ボタンと動作は同じです。

- #### 値をコピー(4次元)
    カーブの値を`0.70,0.27,0.16,1.00`の形式でクリップボードにコピーします。

- #### カーブを反転
    カーブが、グラフの中心について点対称の形になります。つまり、EaseInのカーブはEaseOutに、EaseOutのカーブはEaseInになります。

### ショートカットキー
カーブ編集時には、以下のショートカットキーが使用できます：

- #### [A]キー
    ペアのハンドルとの角度を固定するかどうかを切り替えます。

- #### [F]キー
    カーブをグラフウィンドウにフィットさせます。

- #### [R]キー
    カーブを反転させます。

- #### [S]キー
    カーブをプリセットに保存します。(未実装)

- #### [<] / [>]キー
    前/次のIDのカーブに移動します。

### 値を読み取る
矢印マークのボタンをクリックし，変換後の値を入力するとグラフにカーブが反映されます。(Valueモードのみ)

### プリセットとして保存(未実装)
マークのボタンをクリックすると設定したカーブをプリセットとして保存でき，プリセットウィンドウから参照することができます。   
設定したプリセットは`curve_editor.bin`に保存されます。

## プリセット(未実装)

## 設定
各種設定をダイアログから行うことができます。  
### 全般
- #### テーマ
    テーマをダーク / ライトの2種類から選ぶことができます。

- #### アラートを表示
    確認や警告等のポップアップウィンドウを表示します。  

- #### 変更前のカーブをグラフに表示
    1ステップ前のカーブの編集状態を表示します。

### 編集

- #### 値を自動でコピー(Valueモード)
    ハンドルを動かしたときにカーブの値が自動でクリップボードにコピーされます。



## その他
### Valueモードの注意点
- グラフでの制御点のY座標(グラフの下端を0、上端を1としたとき)は-2.74~3.74まで有効です。これより外の値は設定できません。  

- Valueモードではグラフの一括変更ができないため，カーブを使い回す場合はあらかじめ調整をしてからの方が良いかもしれません。  

- Valueモードでは小数点以下2桁刻みでカーブを設定するため，`.flow`ファイルを読み込む際，値が小数点以下3桁まで記述されている場合は四捨五入されます。  

### IDモードの注意点
- IDモードで設定したカーブは，エイリアス(`.exa`)やオブジェクトファイル(`.exo`)として出力しても保存されません。  

- IDモードでは，`.flow`ファイルの読み込みはできますが書き出しができない仕様となっています。  

- `Value@Curve Editor`はプラグインと連動していないため，プラグインを入れていなくても動作します。   

## 動作環境
| OS | AviUtl | 拡張編集 | 
| :---: | :---: | :---: |
| Windows 11 | ~1.10 | ~0.93rc(0.92推奨) |

【動作確認済み】  
Windows 11  
AviUtl 1.00, 1.10  
拡張編集 0.92  

## 免責事項
当プラグイン・スクリプトを使用したことによっていかなる損害が生じても，製作者は一切の責任を負わないものとします。  


