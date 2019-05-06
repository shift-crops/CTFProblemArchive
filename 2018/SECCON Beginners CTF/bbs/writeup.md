# 想定解法

gets 関数による Stack BOF の脆弱性があるため，ROPして system 関数を呼ぶ

## シェル奪取の流れ
1. Stack BOF を利用して gets@plt を呼び，適当な固定アドレスに `/bin/sh` を配置
2. 続いてそのアドレスを引数に与えて system@plt を呼ぶ
