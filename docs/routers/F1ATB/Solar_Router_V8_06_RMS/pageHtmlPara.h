//***************************************************
// Page HTML et Javascript de gestion des Paramètres
//***************************************************
const char *ParaHtml = R"====(
  <!doctype html>
  <html><head><meta charset="UTF-8"><style>
    * {box-sizing: border-box;}
    body {font-size:150%;text-align:center;width:100%;max-width:1000px;margin:auto;background: linear-gradient(#003,#77b5fe,#003);background-attachment:fixed;color:white;}
    h2{text-align:center;color:white;}
    a:link {color:#aaf;text-decoration: none;}
    a:visited {color:#ccf;text-decoration: none;}
    .form {margin:auto;padding:10px;display: table;text-align:left;width:100%;}
    .ligne {display: table-row;padding:10px;}
    label,input{display: table-cell;margin: 5px;text-align:left;font-size:20px;height:25px;}
    .boldT{text-align:left;font-weight:bold;display: table-row;}
    .onglets{margin-top:4px;left:0px;font-size:130%;}
    .Baccueil,.Bbrut,.Bparametres,.Bactions{margin-left:20px;border:outset 4px grey;background-color:#333;border-radius:6px;padding-left:20px;padding-right:20px;display:inline-block;}
    .Bparametres{border:inset 10px azure;}
    #BoutonsBas{display:flex;justify-content:space-between;margin-top:20px;}
    .pied{display:flex;justify-content:space-between;font-size:14px;color:white;}
    #ligneFixe,#ligneTemperature,#Tui,#CUi,#CuI,#ligneExt,#ligneEnphaseUser,#ligneEnphasePwd,#ligneEnphaseSerial{display:none;}
    .fsize10{font-size:10px;height:14px;}
  </style>
  <script src="/ParaJS"></script>
  <script src="/ParaRouteurJS"></script>
  </head>
  <body onLoad="Init();">
    <div class='onglets'><div class='Baccueil'><a href='/'>Accueil</a></div><div class='Bbrut'><a href='/Brute'>Donn&eacute;es brutes</a></div><div class='Bparametres'><a href='/Para'>Param&egrave;tres</a></div><div class='Bactions'><a href='/Actions'>Actions</a></div></div>
    <h2 id='nom_R'>Routeur Solaire - RMS</h2><h4>Param&egrave;tres</h4>
    <div class="boldT"><br>Source des mesures</div>
    <div class="form"  >
      <div class="ligne">
        <label for='UxI' style='text-align:right;'>UxI</label>
        <input type='radio' name='sources' id='UxI' value="UxI"  onclick="checkDisabled();">
        <label for='UxIx2' style='text-align:right;'>UxIx2</label>
        <input type='radio' name='sources' id='UxIx2' value="UxIx2"  onclick="checkDisabled();">       
        <label for='Linky' style='text-align:right;'>Linky</label>
        <input type='radio' name='sources' id='Linky' value="Linky"  onclick="checkDisabled();">
        <label for='Enphase' style='text-align:right;'>Enphase-Envoy</label>
        <input type='radio' name='sources' id='Enphase' value="Enphase"  onclick="checkDisabled();">
        <label for='SmartG' style='text-align:right;'>SmartGateways (en test)</label>
        <input type='radio' name='sources' id='SmartG' value="SmartG"  onclick="checkDisabled();">
        <label for='ShellyEm' style='text-align:right;'>Shelly Em</label>
        <input type='radio' name='sources' id='ShellyEm' value="ShellyEm"  onclick="checkDisabled();">
        <label for='Ext' style='text-align:right;'>ESP Externe</label>
        <input type='radio' name='sources' id='Ext' value="Ext"  onclick="checkDisabled();">
      </div>
      <div><span class='fsize10'>N&eacute;cessite un Reset de l'ESP32</span></div>
    </div>
    <div class="form"  >
      <div class='ligne' id="ligneExt">
        <label for='RMSextIP'>Adresse IP <span id='labExtIp'></span> externe (ex : 192.168.1.248) : </label>
        <input type='text' name='RMSextIP' id='RMSextIP' >
      </div>
      <div class='ligne' id="ligneEnphaseUser">
        <label for='EnphaseUser'>Enphase Envoye-S metered User : <span class='fsize10'><br>Pour firmvare Envoy-S V7 seulement</span></label>
        <input type='text' name='EnphaseUser' id='EnphaseUser' >
      </div>
      <div class='ligne' id="ligneEnphasePwd">
        <label for='EnphasePwd'>Enphase Envoye-S metered Password : <span class='fsize10'><br>Pour firmvare Envoy-S V7 seulement</span></label>
        <input type='password' name='EnphasePwd' id='EnphasePwd' >
      </div>
      <div class='ligne' id="ligneEnphaseSerial">
        <label for='EnphaseSerial' id="label_enphase_shelly"></label>
        <input type='text' name='EnphaseSerial' id='EnphaseSerial' onchange='checkDisabled();'>
      </div>
      <br>
      <div class='ligne boldT'>
        <label for='nomRouteur' >Nom du routeur : </label>
        <input type='text' name='nomRouteur' id='nomRouteur' >
      </div>
      <div class='ligne boldT' id='ligneMobile'>
        <label for='nomSondeMobile' >Nom Données courant Maison : </label>
        <input type='text' name='nomSondeMobile' id='nomSondeMobile' >
      </div>
      <div class='ligne boldT' id='ligneFixe'>
        <label for='nomSondeFixe' >Nom Données courant seconde sonde : </label>
        <input type='text' name='nomSondeFixe' id='nomSondeFixe' >
      </div>
      <div class='ligne boldT' id='ligneTemperature'>
        <label for='nomTemperature' >Nom Température : </label>
        <input type='text' name='nomTemperature' id='nomTemperature' >
      </div>
      <div class='ligne boldT'>
        <label for='TempoEDFon'>Couleur Tempo EDF : </label>
        <input type='checkbox' name='TempoEDFon' id='TempoEDFon' style='width:25px;' >
      </div>
      <br>
      <div class="boldT">Adresse IP de l'ESP</div>
      <div class='ligne'>
        <label for='dhcp'>Adresse IP auto (DHCP) : </label>
        <input type='checkbox' name='dhcp' id='dhcp' style='width:25px;' onclick="checkDisabled();">
      </div>
      <div class='ligne'>
        <label for='adrIP'>Adresse IP si fixe (ex : 192.168.1.245) : <br><span class='fsize10'>N&eacute;cessite un Reset de l'ESP32</span></label>
        <input type='text' name='adrIP' id='adrIP' >
      </div>
      <div class='ligne'>
        <label for='gateway'>Passerelle / Gateway (ex : 192.168.1.254) :  <br><span class='fsize10'>En g&eacute;n&eacute;ral l'adresse de votre box internet</span></label>
        <input type='text' name='gateway' id='gateway' >
      </div>
      <div class='ligne'>
        <label for='masque'>Masque / Subnet (ex : 255.255.255.0) :  </label>
        <input type='text' name='masque' id='masque' >
      </div>
      <div class='ligne'>
        <label for='dns'>DNS (ex : 192.168.1.254) :  <br><span class='fsize10'>En g&eacute;n&eacute;ral l'adresse de votre box internet</span></label>
        <input type='text' name='dns' id='dns' >
      </div>
      
      <div class="boldT"><br>Envoi Puissance au serveur MQTT <small>(Home Assistant , Domoticz ...)</small></div>
      <div class='ligne'>
        <label for='MQTTRepete'>P&eacute;riode (s) r&eacute;petition  (0= pas d'envoi) : </label>
        <input type='number' name='MQTTRepete' id='MQTTRepete'  onclick="checkDisabled();" >
      </div>
      <div class='ligne'>
        <label for='MQTTIP'>Adresse IP host MQTT (ex : 192.168.1.18) : </label>
        <input type='text' name='MQTTIP' id='MQTTIP' >
      </div>
      <div class='ligne'>
        <label for='MQTTPort'> port (ex : 1883) : </label>
        <input type='number' name='MQTTPort' id='MQTTPort' >
      </div>
      <div class='ligne'>
        <label for='MQTTUser'>MQTT User nom : </label>
        <input type='text' name='MQTTUser' id='MQTTUser' >
      </div>
      <div class='ligne'>
        <label for='MQTTpwd'>MQTT mot de passe : </label>
        <input type='password' name='MQTTpwd' id='MQTTpwd' >
      </div>
      <div class='ligne'>
        <label for='MQTTPrefix'>MQTT Prefix (1 seul mot ex : homeassistant ) : </label>
        <input type='text' name='MQTTPrefix' id='MQTTPrefix' >
      </div>
      <div class='ligne'>
        <label for='MQTTdeviceName'>MQTT Device Name (1 seul mot ex : routeur_rms ) : </label>
        <input type='text' name='MQTTdeviceName' id='MQTTdeviceName' >
      </div>
      
      <div class="boldT" id='Tui'><br>Calibration Mesures Ueff et Ieff</div>
      <div class='ligne' id='CUi'>
        <label for='CalibU'>Coefficient multiplicateur Ueff (typique : 1000) : </label>
        <input type='number' name='CalibU' id='CalibU'   >
      </div>
      <div class='ligne' id='CuI'>
        <label for='CalibI'>Coefficient multiplicateur Ieff (typique : 1000) : </label>
        <input type='number' name='CalibI' id='CalibI'   >
      </div>
    </div>
    <div  id='BoutonsBas'>
        <input type='button' onclick='Reset();' value='ESP32 Reset' >
        <input type='button' onclick="SendValues();" value='Sauvegarder' >
    </div>
    <br>
    <div class='pied'><div>Routeur Version : <span id='version'></span></div><div><a href='https:F1ATB.fr' >F1ATB.fr</a></div></div>
    <br>
  </body></html>
)====";
const char *ParaJS = R"====(
  var LaTemperature = -100;
  function Init(){
    LoadParametres();
    LoadParaRouteur();
  }
  function LoadParametres() {
    var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() { 
          if (this.readyState == 4 && this.status == 200) {
             var LesParas=this.responseText;
             var Para=LesParas.split(RS);  
             GID("dhcp").checked = Para[0]==1  ? true:false;
             GID("adrIP").value=int2ip(Para[1]);
             GID("gateway").value=int2ip(Para[2]);
             GID("masque").value=int2ip(Para[3]);
             GID("dns").value=int2ip(Para[4]);
             GID(Para[5]).checked = true;
             GID("RMSextIP").value=int2ip(Para[6]);
             GID("EnphaseUser").value=Para[7];
             GID("EnphasePwd").value=Para[8];
             GID("EnphaseSerial").value=Para[9];
             GID("MQTTRepete").value = Para[10];
             GID("MQTTIP").value=int2ip(Para[11]);
             GID("MQTTPort").value=Para[12];
             GID("MQTTUser").value=Para[13];
             GID("MQTTpwd").value=Para[14];
             GID("MQTTPrefix").value=Para[15];
             GID("MQTTdeviceName").value=Para[16];
             GID("nomRouteur").value=Para[17];
             GID("nomSondeFixe").value=Para[18];
             GID("nomSondeMobile").value=Para[19];
             LaTemperature=parseInt(Para[20]);
             GID("nomTemperature").value=Para[21];
             GID("CalibU").value=Para[22];
             GID("CalibI").value=Para[23];
             GID("TempoEDFon").checked = Para[24]==1  ? true:false;
             
             checkDisabled();
          }         
        };
        xhttp.open('GET', 'ParaAjax', true);
        xhttp.send();
      }
  function SendValues(){
    var dhcp=GID("dhcp").checked ? 1:0;
    var TempoEDFon=GID("TempoEDFon").checked ? 1:0;
    var Source_new=document.querySelector('input[name="sources"]:checked').value;
    var S=dhcp+RS+ ip2int(GID("adrIP").value)+RS+ ip2int(GID("gateway").value);
    S +=RS+ip2int(GID("masque").value)+RS+ ip2int(GID("dns").value)
    S +=RS+Source_new+RS+ ip2int(GID("RMSextIP").value)+ RS+GID("EnphaseUser").value.trim()+RS+GID("EnphasePwd").value.trim()+RS+GID("EnphaseSerial").value.trim();
    S +=RS+GID("MQTTRepete").value +RS+ip2int(GID("MQTTIP").value) +RS+GID("MQTTPort").value +RS+GID("MQTTUser").value.trim()+RS+GID("MQTTpwd").value.trim();
    S +=RS+GID("MQTTPrefix").value.trim()+RS+GID("MQTTdeviceName").value.trim()+RS+GID("nomRouteur").value.trim()+RS+GID("nomSondeFixe").value.trim()+RS+GID("nomSondeMobile").value.trim();
    S +=RS+GID("nomTemperature").value.trim();
    S +=RS+GID("CalibU").value+RS+GID("CalibI").value + RS + TempoEDFon;
    S="?lesparas="+clean(S);
    if ((GID("dhcp").checked ||  checkIP("adrIP")&&checkIP("gateway"))   && (!GID("MQTTRepete").checked ||  checkIP("MQTTIP"))){
      var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() { 
          if (this.readyState == 4 && this.status == 200) {
            var retour=this.responseText;
            location.reload();
          }         
        };
        xhttp.open('GET', 'ParaUpdate'+S, true);
        xhttp.send();
    }
  }
  function checkDisabled(){ 
    GID("adrIP").disabled=GID("dhcp").checked;
    GID("gateway").disabled=GID("dhcp").checked;
    GID("masque").disabled=GID("dhcp").checked;
    GID("dns").disabled=GID("dhcp").checked;
    GID("RMSextIP").disabled=!GID("Ext").checked&&!GID("Enphase").checked&&!GID("SmartG").checked&&!GID("ShellyEm").checked;
    GID("MQTTIP").disabled=GID("MQTTRepete").value ==0?true:false;
    GID("MQTTPort").disabled=GID("MQTTRepete").value ==0?true:false;
    GID("MQTTUser").disabled=GID("MQTTRepete").value ==0?true:false;
    GID("MQTTpwd").disabled=GID("MQTTRepete").value ==0?true:false; 
    GID("MQTTPrefix").disabled=GID("MQTTRepete").value ==0?true:false; 
    GID("MQTTdeviceName").disabled=GID("MQTTRepete").value ==0?true:false; 
    GID('ligneTemperature').style.display = (LaTemperature>-100) ? "table-row" : "none";
    Source = document.querySelector('input[name="sources"]:checked').value;
    if (Source !='Ext') Source_data=Source;
    AdaptationSource();
  }
  function checkIP(id){
    var S=GID(id).value;
    var Table=S.split(".");
    var valide=true;
    if (Table.length!=4) {
      valide=false;
    }else{
      for (var i=0;i<Table.length;i++){
        if (Table[i]>255 || Table[i]<0) valide=false;
      }
    }
    if (valide){
      GID(id).style.color="black";
    } else {
      GID(id).style.color="red";
    }
    return valide;
  }
 
  
  function Reset(){
      var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() { 
          if (this.readyState == 4 && this.status == 200) {
            GID('BoutonsBas').innerHTML=this.responseText;
            setTimeout(location.reload(),3000);
          }         
        };
        xhttp.open('GET', 'restart', true);
        xhttp.send();
  }
  function AdaptationSource(){
      GID('ligneFixe').style.display = (Source_data=='UxIx2' || (Source_data=='ShellyEm' && GID("EnphaseSerial").value <3))? "table-row" : "none";
      if (Source_data=='UxI' && Source=='UxI' ) {
        GID('Tui').style.display="table-row";
        GID('CUi').style.display="table-row";
        GID('CuI').style.display="table-row";
      } else {
        GID('Tui').style.display="none";
        GID('CUi').style.display="none";
        GID('CuI').style.display="none";
      }
      var txtExt = "ESP-RMS";
      if (Source=='Enphase') txtExt = "Enphase-Envoy";
      if (Source=='SmartG') txtExt = "SmartGateways";
      var lab_enphaseShelly= "Numéro série passerelle IQ Enphase : <span class='fsize10'><br>Pour firmvare Envoy-S V7 seulement</span>";
      if (Source=='ShellyEm') {
        txtExt = "Shelly Em ";
        lab_enphaseShelly="Monophasé : Numéro de voie (0 ou 1) mesurant l'entrée du courant maison<br>Triphasé : mettre 3";
      }
      GID('labExtIp').innerHTML = txtExt;
      GID('label_enphase_shelly').innerHTML = lab_enphaseShelly;
      GID('ligneExt').style.display = (Source=='Ext' || Source=='Enphase' || Source=='SmartG' || Source=='ShellyEm') ? "table-row" : "none";
      GID('ligneEnphaseUser').style.display = (Source=='Enphase') ? "table-row" : "none";
      GID('ligneEnphasePwd').style.display = (Source=='Enphase') ? "table-row" : "none";
      GID('ligneEnphaseSerial').style.display = (Source=='Enphase' || Source=='ShellyEm') ? "table-row" : "none"; //Numéro de serie ou voie
  }
)====";

//Paramètres du routeur et fonctions générales pour toutes les pages.
const char *ParaRouteurJS = R"====(
  var Source="";
  var Source_data="";
  var RMSextIP="";
  var GS=String.fromCharCode(29); //Group Separator
  var RS=String.fromCharCode(30); //Record Separator
  var nomSondeFixe="Sonde Fixe";
  var nomSondeMobile="Sonde Mobile";
  var nomTemperature="Temperature"; 
  function LoadParaRouteur() {
    var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() { 
          if (this.readyState == 4 && this.status == 200) {
             var LesParas=this.responseText;
             var Para=LesParas.split(RS);
             Source=Para[0];
             Source_data=Para[1];
             RMSextIP= Para[6]; 
             AdaptationSource();  
             GH("nom_R",Para[2]);
             GH("version",Para[3]);
             GH("nomSondeFixe",Para[4]);
             GH("nomSondeMobile",Para[5]); 
             nomSondeFixe=Para[4];
             nomSondeMobile=Para[5];
             nomTemperature=Para[7]; 
               
          }         
        };
        xhttp.open('GET', 'ParaRouteurAjax', true);
        xhttp.send();
  }
  function GID(id) { return document.getElementById(id); };
  function GH(id, T) {
    if ( GID(id)){
     GID(id).innerHTML = T; }
    }
  function GV(id, T) { GID(id).value = T; }
  function clean(S){ //Remplace & et ? pour les envois au serveur
    let res=S.replace(/\%/g,"%25");
    res = res.replace(/\&/g, "%26");
    res = res.replace(/\#/g, "%23");
    res = res.replace(/\+/g, "%2B");
    res=res.replace(/amp;/g,"");
    return res.replace(/\?/g,"%3F");
  }
  function int2ip (V) {
    var ipInt=parseInt(V);
    return ( (ipInt>>>24) +'.' + (ipInt>>16 & 255) +'.' + (ipInt>>8 & 255) +'.' + (ipInt & 255) );
  }
  function ip2int(ip) {
    ip=ip.trim();
    return ip.split('.').reduce(function(ipInt, octet) { return (ipInt<<8) + parseInt(octet, 10)}, 0) >>> 0;
  }
  
)====";