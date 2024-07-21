class PageController {
  constructor() {
    this.initEventListeners();
    this.loadData();
    this.startWeightUpdate();
  }

  loadData() {
    // ページが読み込まれた時にESPからデータを取得して表示
    fetch('/getData')
    .then(response => response.json())
    .then(data => {
      document.getElementById('ssid').placeholder = data.ssid;
      // document.getElementById('ps').placeholder = data.ps;
      document.getElementById('gasId').placeholder = data.gasId;
      document.getElementById(data.nfcType).checked = true;
      PageController.nfcType = data.nfcType;
      document.getElementById('setWeight').textContent = parseFloat(data.palletWeight).toFixed(2);

    });
  }

  initEventListeners() {
    // イベントリスナーの初期化
    document.getElementById('ssidupdateForm').addEventListener('submit', event => {
      event.preventDefault();
      this.updateData('ssid');
    });
    document.getElementById('psupdateForm').addEventListener('submit', event => {
      event.preventDefault();
      this.updateData('ps');
    });
    document.getElementById('gasIdupdateForm').addEventListener('submit', event => {
      event.preventDefault();
      this.updateData('gasId');
    });

    document.querySelectorAll('input[type="radio"][name="nfcType"]').forEach(radio => {
      radio.addEventListener('click', () => this.updateUserType(radio.id));
    });

    document.getElementById('palletForm').addEventListener('submit', event => {
      event.preventDefault();
      this.setWeight();
    });

    document.getElementById('restartButton').addEventListener('click', () => this.restart());

    document.getElementById('confirmButton').addEventListener('click', () => this.confirmRestart());
    document.getElementById('cancelButton').addEventListener('click', () => this.closeModal());
  }

  startWeightUpdate() {
    setInterval(() => {
      fetch('/getWeight')
      .then(response => response.json())
      .then(data => {
        document.getElementById('nowWeight').textContent = parseFloat(data.weight).toFixed(2);
        if(data.stable === "ST"){
          document.getElementById("palletButton").disabled = false;
        }else{
          document.getElementById("palletButton").disabled = true;//不安定時無効にしたいが上手く動かない？
        }
      })
      .catch(error => console.error('Error:', error));
    }, 500); // 500mSごとに更新
  }

  updateData(type) {
    var newData = document.getElementById(type).value;
    if (newData) {
      const controller = new AbortController(); // タイムアウト用のコントローラーを作成
      const timeoutId = setTimeout(() => controller.abort(), 5000); // 5秒後にリクエストを中止

      fetch('/' + type + 'Update', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/x-www-form-urlencoded',
        },
        body: 'newData=' + encodeURIComponent(newData),
        signal: controller.signal, // タイムアウト用のシグナルを追加
      }).then(response => {
        clearTimeout(timeoutId); // タイムアウトタイマーをクリア
        if(response.status == 503){
          this.customMessage('サーバーが応答しません。後ほど再試行してください。');
        }else{
          document.getElementById(type).value = "";
          document.getElementById(type).placeholder = newData;
        }
      }).catch(error => {
        clearTimeout(timeoutId); // タイムアウトタイマーをクリア
        console.error('Error during update request:', error);
        this.customMessage("接続に失敗しました。:\n" + error.message);
      });
    }
  }

  setWeight(){
    var newData = document.getElementById('nowWeight').textContent;
    const controller = new AbortController(); // タイムアウト用のコントローラーを作成
    const timeoutId = setTimeout(() => controller.abort(), 5000); // 5秒後にリクエストを中止
    fetch('/setWeight', {
      method: 'POST',
      headers: {
        'Content-Type': 'application/x-www-form-urlencoded',
      },
      body: 'newData=' + newData,
      signal: controller.signal, // タイムアウト用のシグナルを追加
    }).then(response => {
      clearTimeout(timeoutId); // タイムアウトタイマーをクリア
      if(response.status == 503){
        this.customMessage('サーバーが応答しません。後ほど再試行してください。');
      }else{
        document.getElementById('setWeight').textContent = newData;
      }
    }).catch(error => {
      clearTimeout(timeoutId); // タイムアウトタイマーをクリア
      console.error('Error during setWeight request:', error);
      this.customMessage("接続に失敗しました。:\n" + error.message);
    });
  }

  updateUserType(id) {
    const controller = new AbortController(); // タイムアウト用のコントローラーを作成
    const timeoutId = setTimeout(() => controller.abort(), 5000); // 5秒後にリクエストを中止
    
    fetch('/nfcTypeUpdate', {
      method: 'POST',
      headers: {
        'Content-Type': 'application/x-www-form-urlencoded',
      },
      body: 'newData=' + encodeURIComponent(id),
      signal: controller.signal, // タイムアウト用のシグナルを追加
    }).then(response => {
      clearTimeout(timeoutId); // タイムアウトタイマーをクリア
      if(response.status == 503){
        this.customMessage('サーバーが応答しません。後ほど再試行してください。');
        document.getElementById(PageController.nfcType).checked = true; // エラー時にチェックを戻す
      }else{
        PageController.nfcType = id;  // グローバル変数に選択された値を保存
        document.getElementById(PageController.nfcType).checked = true;
      }
    }).catch(error => {
      clearTimeout(timeoutId); // タイムアウトタイマーをクリア
      console.error('Error during NFC type update request:', error);
      this.customMessage("接続に失敗しました。:\n" + error.message);
      document.getElementById(PageController.nfcType).checked = true; // エラー時にチェックを戻す
    });
  }

  restart() {
    document.getElementById('modalMessage').textContent = "再起動しますか？";
    document.getElementById('confirmButton').style.display = 'block';
    document.getElementById('cancelButton').textContent = 'いいえ';
    document.getElementById('cancelButton').onclick = () => this.closeModal();
    this.openModal();
  }

  confirmRestart() {
    const controller = new AbortController(); // タイムアウト用のコントローラーを作成
    const timeoutId = setTimeout(() => controller.abort(), 5000); // 5秒後にリクエストを中止
    fetch('/restart', {
      method: 'POST',
      signal: controller.signal, // タイムアウト用のシグナルを追加
    }).then(response => {
      clearTimeout(timeoutId); // タイムアウトタイマーをクリア
      if(response.status == 503){
        this.customMessage('サーバーが応答しません。後ほど再試行してください。');
      }else{
        this.closeModal();
        this.customMessage('再起動しています...')
      }
      
    }).catch(error => {
      clearTimeout(timeoutId); // タイムアウトタイマーをクリア
      console.error('Error during restart request:', error);
      this.customMessage("接続に失敗しました。:\n" + error.message);
    });
  }

  openModal() {
    document.getElementById('customModal').style.display = 'block';
  }

  closeModal() {
    document.getElementById('customModal').style.display = 'none';
  }

  customMessage(message) {
    document.getElementById('modalMessage').textContent = message;
    document.getElementById('confirmButton').style.display = 'none';
    document.getElementById('cancelButton').textContent = 'OK';
    document.getElementById('cancelButton').onclick = () => this.closeModal();
    this.openModal();
  }
}

new PageController();