// **********************************************************
//  Page de connexion 'Acces Point' pour définir réseau WIFI
// **********************************************************
const char *ConnectAP_Html = R"====(
<!doctype html>
<html><head><meta charset="UTF-8">
    <script src="/ParaRouteurJS"></script>
    <style>
      body {font-size:150%;text-align:center;width:1000px;margin:auto;background: linear-gradient(#003,#77b5fe,#003);background-attachment:fixed;color:white;padding:4px;}
      #form-passe {display: none;padding:10px;text-align:center;margin:auto;width:100%;}
      label,input,.dB {display: table-cell;padding:2px;text-align:left;font-size:120%;}
      .l0{display:table-row;margin:auto;background-color:#333;padding:2px;}
      .l1{display:table-row;margin:auto;background-color:#666;padding:2px;}
      #ListeWifi{display:inline-block;margin:auto;}
      #envoyer{padding-top:20px;display:none;}
      #attente2{display:none;}
    </style>
</head>
<body>
<h1>Routeur Solaire - RMS</h1><h4>Connexion au r&eacute;seau WIFI local</h4>
<div id="ListeWifi"></div><br><br>
<div id="scanReseau">
    <input type='button' onclick="ScanWIFI();" value='Scan réseaux WIFI' >
</div>
<br>
<div id='form-passe'>
  <div >Entrez le mot de passe du r&eacute;seau : </div>
  <input type='password' name='passe' id='passe' required>
</div>
<div id="envoyer" >
    <input type='button' onclick="Envoyer();" value='Envoyer' >
</div>
<br>
<div id="attente2">Attendez l'adresse IP attribuée à l'ESP 32</div>
<br>
<script>
  function ScanWIFI(){
    GH("ListeWifi", "Patientez 10s");
    var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() { 
          if (this.readyState == 4 && this.status == 200) {
             var LesWifi=this.responseText;
             var Wifi=LesWifi.split(GS); 
             var S="Sélectionnez un réseau Wifi";
             for (var i=0;i<Wifi.length-1;i++) {
                var wifi=Wifi[i].split(RS); 
                var j=i%2;
                S +="<div class='l"+ j +"'>";
                S +="<label for='W" + i+"'>" + wifi[0] +" </label>";
                S +="<div class='dB'>" +wifi[1] +" dBm</div> <input type='radio' name='Wifi' value='" +  wifi[0] +"' onclick='ChoixWifi(this.value);'>";
                S +="</div>";
              }
              
              
             GH("ListeWifi", S);
          }         
        };
        xhttp.open('GET', 'AP_ScanWifi', true);
        xhttp.send();
        GID("form-passe").style.display = "none";
        GID("envoyer").style.display = "none";
        GID("attente2").style.display = "none";
  }
  function Envoyer(){
    var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() { 
          if (this.readyState == 4 && this.status == 200) {
             var Resp=this.responseText;
             var Txt=Resp.split(RS); 
             GH("ListeWifi", Txt[1]);
             GID("attente2").style.display = "none";
             GID("form-passe").style.display = "none";
             GID("envoyer").style.display = "none";
             if (Txt[0] == "Ok") {
              GID("scanReseau").style.display = "none";      
             }
          }         
        };
        var adr ="/AP_SetWifi?ssid="+clean(ssid)+"&passe=" + clean(GID("passe").value);
        xhttp.open('GET', adr, true);
        xhttp.send();
        GID("form-passe").style.display = "none";
        GID("envoyer").style.display = "none";
        GID("attente2").style.display = "block";
  }
  ssid="";
  function ChoixWifi(V){
      ssid = V;
      GID("envoyer").style.display = "inline-block";
      GID("form-passe").style.display = "inline-block";
  }
</script>
</body></html>
)====";
