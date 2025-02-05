class PageController {
  constructor() {
    this.initEventListeners();
    this.loadData();
  }

  loadData() {
    // ページが読み込まれた時にESPからデータを取得して表示
    fetch('/getData')
    .then(response => response.json())
    .then(data => {
      document.getElementById('time1').placeholder = data.time1;
      document.getElementById('time2').placeholder = data.time2;
      document.getElementById('time3').placeholder = data.time3;
    });
  }

  initEventListeners() {
    // イベントリスナーの初期化
    document.getElementById('time1From').addEventListener('submit', event => {
      event.preventDefault();
      this.updateData('time1');
    });
    document.getElementById('time2From').addEventListener('submit', event => {
      event.preventDefault();
      this.updateData('time2');
    });
    document.getElementById('time3From').addEventListener('submit', event => {
      event.preventDefault();
      this.updateData('time3');
    });
    
    document.getElementById('time1').addEventListener('keypress', event => this.allowOnlyNumbers(event));
    document.getElementById('time2').addEventListener('keypress', event => this.allowOnlyNumbers(event));
    document.getElementById('time3').addEventListener('keypress', event => this.allowOnlyNumbers(event));
    document.getElementById('restartButton').addEventListener('click', () => this.restart());

    document.getElementById('confirmButton').addEventListener('click', () => this.confirmRestart());
    document.getElementById('cancelButton').addEventListener('click', () => this.closeModal());
  }

  allowOnlyNumbers(evt) {
    // ASCII '0'-'9' の範囲をチェック
    var charCode = (evt.which) ? evt.which : evt.keyCode;
    if (charCode > 31 && (charCode < 48 || charCode > 57)){
      console.log('Only numbers are allowed');
      evt.preventDefault();
      return false;
    }
    console.log('Number entered');
    return true;
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