# Bluepad32・DualShock 接続と入力取得メモ

このプロジェクトの `src/main.cpp` にある `gamepad`、`controllerInput`、`input` を使う例です。
以下は追加・置き換え用のコードです。このメモの作成時点では、入力取得処理を `main.cpp` へ反映していません。

## 入力を取得するメンバ関数

接続済みの `ControllerPtr gamepad` に対して呼び出します。

| 操作 | 関数 | 戻り値の型 | 値の範囲・意味 | 保存先 |
|---|---|---|---|---|
| 左スティックX | `gamepad->axisX()` | `int32_t` | 約 −512〜512、中央は約0 | `input.leftX` |
| 左スティックY | `gamepad->axisY()` | `int32_t` | 約 −512〜512、中央は約0 | `input.leftY` |
| 右スティックX | `gamepad->axisRX()` | `int32_t` | 約 −512〜512、中央は約0 | `input.rightX` |
| 右スティックY | `gamepad->axisRY()` | `int32_t` | 約 −512〜512、中央は約0 | `input.rightY` |
スティック→中央値４，-508~512
左、上→マイナス
| L2トリガー | `gamepad->brake()` | `int32_t` | 0〜1020、離すと0、最大押し込みで1020 | `input.leftAxis` |
| R2トリガー | `gamepad->throttle()` | `int32_t` | 0〜1020、離すと0、最大押し込みで1020 | `input.rightAxis` |
| L1ボタン | `gamepad->l1()` | `bool` | 押下で `true` | `input.leftbutton` |
| R1ボタン | `gamepad->r1()` | `bool` | 押下で `true` | `input.rightbutton` |
| 十字キー | `gamepad->dpad()` | `uint8_t` | ビットマスク。未操作は0 | `input.dpad` |
左：0x08、右：0x04、上：0x01、下：0x02
ボタン
〇：0x002、□：0x004、△：0x008、✕：0x001

スティックは通常、右・下がプラス、左・上がマイナスです。
同梱の公式サンプルは軸を −511〜512 と表記しています。端点や中心値には機種差があるため、上の表は概数です。端点との完全一致ではなく、しきい値で判定してください。

十字キーは上=`0x01`、下=`0x02`、右=`0x04`、左=`0x08`。斜めはビットの組み合わせです。
例えば上方向を含むかは `(input.dpad & 0x01) != 0` で判定します。

## 軸とボタンを取得する追加コード

次の関数を `loop()` より前に追加します。既存の `input` と `gamepad` の宣言より後に置いてください。

```cpp
void readController() {
  if (!gamepad || !gamepad->isConnected()) {
    return;
  }

  input.leftX = gamepad->axisX();
  input.leftY = gamepad->axisY();
  input.rightX = gamepad->axisRX();
  input.rightY = gamepad->axisRY();

  input.leftAxis = gamepad->brake();      // L2
  input.rightAxis = gamepad->throttle();  // R2

  input.leftbutton = gamepad->l1();
  input.rightbutton = gamepad->r1();
  input.dpad = gamepad->dpad();
}
```

既存の `loop()` を次に置き換えます。`loop()` を2つ定義しないでください。

```cpp
void loop() {
  if (BP32.update()) {
    readController();
  }
  delay(10);
}
```

`BP32.update()` は受信データを更新し、更新があると `true` を返します。
切断時には、既存の `onControllerDisconnected()` が `input = controllerInput{};` で入力値と接続状態を初期化します。

### 中央付近の揺れを無視する例（デッドゾーン）

必要に応じて、各軸の取得直後に追加します。30は調整用の例です。

```cpp
if (abs(input.leftX) < 30) input.leftX = 0;
if (abs(input.leftY) < 30) input.leftY = 0;
if (abs(input.rightX) < 30) input.rightX = 0;
if (abs(input.rightY) < 30) input.rightY = 0;
```

## 接続処理と、接続できないときの追加コード

通常の初期化は既存の `setup()` にあります。

```cpp
BP32.setup(onControllerConnected, onControllerDisconnected);
BP32.enableNewBluetoothConnections(true);
```

過去のペアリング情報が原因で接続できない場合は、次のように一時的に消去できます。
保存済みの全コントローラーのペアリング情報を消す操作です。

```cpp
BP32.setup(onControllerConnected, onControllerDisconnected);
BP32.forgetBluetoothKeys();  // 復旧用。通常運用では削除する
BP32.enableNewBluetoothConnections(true);
```

書き込み後に再ペアリングを試します。復旧後は `BP32.forgetBluetoothKeys();` を削除して再度書き込んでください。残すと起動するたびにペアリング情報が消えます。

### DualShock 4のペアリング手順

1. PlatformIOでUploadを実行する（Buildだけでは実機は更新されない）。
2. シリアルモニターを115200bpsで開き、ESPをリセットする。
3. `[BT] Waiting for a gamepad...` が出ることを確認する。
4. コントローラーのUSBケーブルを外し、電源を切る。
5. SHAREを押しながらPSボタンを、ライトが点滅するまで長押しする。
6. `[BT] Gamepad connected: ...` が出れば、このコードの接続コールバックまで到達している。

改善しない場合は、以前接続したPCやPS4への再接続を避けて試します。
コントローラー側をリセットする場合は、USBを外して背面の小穴にあるボタンを5秒以上押し、その後に再ペアリングします。

DualShock 3は別手順です。USB経由でESPのBluetoothアドレスを登録する手動ペアリングが必要です。

## 参照

- インストール済みBluepad32の `ArduinoController.h` と `Controller.ino`（関数・入力取得例）
- [Bluepad32 対応コントローラー](https://bluepad32.readthedocs.io/en/latest/supported_gamepads/)
- [Bluepad32 Arduino API](https://bluepad32.readthedocs.io/en/latest/programmers_guide_arduino/)（最新版と本プロジェクトでは一部のAPI名が異なるため、コード例はインストール済みのAPIに合わせている）
- [Sony DualShock 4 ペアリング手順](https://www.playstation.com/ja-jp/support/hardware/ps4-pair-dualshock-4-wireless-with-pc-or-mac/)
- [Sony DualShock 4 リセット手順](https://www.playstation.com/ja-jp/support/hardware/troubleshoot-dualshock-4/)
- [Bluepad32 DualShock 3 手動ペアリング](https://bluepad32.readthedocs.io/en/latest/pair_ds3/)
