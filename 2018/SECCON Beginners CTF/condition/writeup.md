# 想定解法

名前を入力するバッファの直下にある，フラグを読むための条件変数を書き換える

## フラグ獲得の流れ
1. 0x2cだけパディングの後，`0xdeadbeef` をリトルエンディアンで書き込む