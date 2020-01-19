#!/bin/bash
set -o errexit

echo "=== start deploy data ==="

# set PATH
PATH="$PATH:/opt/eosio/bin"

# cd into script's folder
cd "$(dirname "$0")"

echo "=== start create accounts in blockchain ==="

# скачать JQ для JSON Reader, используем JQ здесь для чтения файла JSON (accounts.json)
mkdir -p ~/bin && curl -sSL -o ~/bin/jq https://github.com/stedolan/jq/releases/download/jq-1.5/jq-linux64 && chmod +x ~/bin/jq && export PATH=$PATH:~/bin

# цикл по массиву в файле json, импорт ключей и создание учетных записей
# эти предварительно созданные учетные записи будут использоваться для сохранения / удаления заметок
# в json мы жестко закодировали каждое имя учетной записи, открытый и закрытый ключи.


jq -c '.[]' accounts.json | while read i; do
  name=$(jq -r '.name' <<< "$i")
  pub=$(jq -r '.publicKey' <<< "$i")

  # для упрощения мы используем один и тот же ключ для владельца и активный ключ каждого аккаунта
  cleos create account eosio $name $pub $pub
done
