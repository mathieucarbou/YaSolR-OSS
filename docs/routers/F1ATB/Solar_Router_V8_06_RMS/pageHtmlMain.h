//************************************************
// Page principale HTML et Javascript 
//************************************************
const char *MainHtml = R"====(
 <!doctype html>
  <html><head><meta charset="UTF-8"><style>
    * {box-sizing: border-box;}
    body {background: linear-gradient(#003,#77b5fe,#003);background-attachment:fixed;font-size:150%;text-align:center;width:100%;max-width:1000px;margin:auto;}
    a:link {color:#aaf;text-decoration: none;}
    a:visited {color:#ccf;text-decoration: none;}
    h2{text-align:center;color:white;}
    .ri { text-align: right;}
    .ce { text-align: center;}
    .blue { background-color:#ddf;}
    .Wh { background-color:#ff8;}
    .W { background-color:#f88;}
    .VA { background-color:aqua;}
    .deg { background-color:#fdf;}
    .temper { background-color:#8f8;}
    .foot { color:white;font-size:16px;}
    .pied{display:flex;justify-content:space-between;font-size:14px;color:white;}
    #date { color:white;}
    .tableau { background-color:white;display:inline-block;margin:auto;padding:4px;}
    table {border:10px inset azure;}
    td { text-align: left;padding:4px;}
    th { text-align: center;padding:4px;}
    svg { border:10px inset azure;background: linear-gradient(#333,#666,#333);}
    #LED{position:absolute;top:4px;left:4px;width:0px;height:0px;border:5px solid red;border-radius:5px;}
    .onglets{margin-top:4px;font-size:130%;}
    .Baccueil,.Bbrut,.Bparametres,.Bactions{margin-left:20px;border:outset 4px grey;background-color:#333;border-radius:6px;padding-left:20px;padding-right:20px;display:inline-block;}
    .Baccueil{border:inset 8px azure;}
    .jauge{background-color:#ff8;height:28px;text-align:left;overflow: visible;position:absolute;top:4px;left:4px;}
    .jaugeBack{background-color:aqua;width:208px;height:36px;position:relative;padding:4px;}
    .w100{width:100%;position:absolute;top:4px;left:4px;}
    .centrer{text-align:center;}
    .dispT,#SVG_PW48hT,#SVG_PW2sT,#SVG_Temp48h{display:none;}
    #donneeDistante,#donneeLocale{font-size:50%;color:white;text-align:center;margin-bottom:10px;display:none;}
    #info{position:absolute;background-color:black;padding:2px;border: 3px inset azure;}
    #couleurTarif_jour,#couleurTarif_J1{font-size:8px;}
  </style></head>
  <body onload='LoadParaRouteur();LoadData();LoadHisto10mn();EtatActions();' >
    <div id='LED'></div>
    <div class='onglets'><div class='Baccueil'><a href='/'>Accueil</a></div><div class='Bbrut'><a href='/Brute'>Donn&eacute;es brutes</a></div><div class='Bparametres'><a href='/Para'>Param&egrave;tres</a></div><div class='Bactions'><a href='/Actions'>Actions</a></div></div>
    <h2 id='nom_R'>Routeur Solaire - RMS</h2>
    <div id='date'>DATE</div>
    <div><div class='tableau'><table >
      <tr class='blue'><th></th><th colspan='2' id='nomSondeMobile'>Maison</th><th colspan='2' class='dispT' id='nomSondeFixe'>Fixe</th><th id='couleurTarif_jour'></th></tr>
      <tr class='blue'><th></th><th>Soutirée</th><th>Injectée</th><th class='dispT'>Conso.</th><th class='dispT' id="produite">Produite</th><th id='couleurTarif_J1'></th></tr>
      <tr class='W'><td>Puissance Active <small>(Pw)</small></td><td id='PwS_M' class='ri'></td><td class='ri' id='PwI_M'></td><td class='dispT ri' id='PwS_T'></td><td class='dispT ri' id='PwI_T'></td><td>W</td></tr>
      <tr class='VA'  id='ligneVA'><td>Puissance Apparente</td><td id='PVAS_M' class='ri'></td><td class='ri'  id='PVAI_M'></td><td class='dispT ri' id='PVAS_T'></td><td class='dispT ri'  id='PVAI_T'></td><td>VA</td></tr>
      <tr class='Wh'><td>Energie Active du jour</td><td id='EAJS_M' class='ri'></td><td id='EAJI_M' class='ri'></td><td class='dispT ri' id='EAJS_T'></td><td class='dispT ri' id='EAJI_T'></td><td>Wh</td></tr>
      <tr class='Wh'><td>Energie Active Totale</td><td id='EAS_M' class='ri'></td><td id='EAI_M' class='ri'></td><td class='dispT ri' id='EAS_T'></td><td class='dispT ri' id='EAI_T'></td><td>Wh</td></tr>
    </table></div></div>
    <div id="donneeDistante">Données distantes</div>
    <div id='etatActions'></div>
    <div id="donneeLocale">Données locales</div>
    <p id='SVG_PW2sM'></p>
    <p id='SVG_PW2sT'></p>
    <p id='SVG_PW48hM'></p>
    <p id='SVG_PW48hT'></p>
    <p id='SVG_Temp48h'></p>
    <p id='SVG_Wh1an'></p>
    <div id='info'></div>
    <br><br><div class='foot' >Donn&eacute;es  RMS<div id='source'></div></div>
    <div class='pied'><div>Routeur Version : <span id='version'></span></div><div><a href='https:F1ATB.fr' >F1ATB.fr</a></div></div>
    <script src='MainJS'></script>
    <script src="/ParaRouteurJS"></script>
    <br></body></html>
)====";

const char *MainJS = R"====(
    var tabPW2sM=[];
    var tabPW2sT=[];
    var initUxIx2=false;
    var biSonde=false;
    var TableauX = [];
    var TableauY0 = [];
    var TableauY1 = [];
    var  myTimeout;
    function LoadData() {
      GID('LED').style='display:block;';
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() { 
        if (this.readyState == 4 && this.status == 200) {
            var DuRMS=this.responseText;
            var groupes=DuRMS.split(GS);
            var G0=groupes[0].split(RS);
            var G1=groupes[1].split(RS);
            var G2=groupes[2].split(RS);
            GID('date').innerHTML = G0[1];
            Source_data= G0[2];
            if (!initUxIx2){
              initUxIx2=true;
              var d='none';
              if(groupes.length==4){ // Cas pour les sources externes UxIx2 et Shelly monophasé
                d="table-cell";
              }
              const collection = document.getElementsByClassName('dispT');
              for (let i = 0; i < collection.length; i++) {
                collection[i].style.display = d;
              }      
            }           
            GID('PwS_M').innerHTML = LaVal(G1[0]); //Maison
            GID('PwI_M').innerHTML = LaVal(G1[1]); //Maison
            GID('PVAS_M').innerHTML = LaVal(G1[2]); //Maison
            GID('PVAI_M').innerHTML = LaVal(G1[3]); //Maison
            GID('EAJS_M').innerHTML = LaVal(G1[4]);
            GID('EAJI_M').innerHTML = LaVal(G1[5]);
            GID('EAS_M').innerHTML = LaVal(G1[6]); 
            GID('EAI_M').innerHTML = LaVal(G1[7]); 
            tabPW2sM.shift(); //Enleve Pw Maison
            tabPW2sM.shift(); //Enleve PVA
            tabPW2sM.push(parseFloat(G1[0]-G1[1]));
            tabPW2sM.push(parseFloat(G1[2]-G1[3]));
            Plot('SVG_PW2sM',tabPW2sM,'#f44','Puissance Active '+GID("nomSondeMobile").innerHTML+' sur 10 mn en W','aqua','Puissance Apparente sur 10 mn en VA');  

            var Tarif=["NON_DEFINI","PLEINE","CREUSE","BLEU","BLANC","ROUGE"];
            var couleur=["#ddf","#f00","#0f0","#00bfff","#fff","#f00"];
            var tarif=["","H.<br>pleine","H.<br>creuse","Tempo<br>Bleu","Tempo<br>Blanc","Tempo<br>Rouge"];
            var idx=0;
            for (i=0;i<6;i++){
              if ( G0[3].indexOf(Tarif[i])>=0){ //LTARF dans Link
                idx=i;
              }
            }
            GID('couleurTarif_jour').style.backgroundColor= couleur[idx];
            GID('couleurTarif_jour').innerHTML =tarif[idx];
            var tempo = parseInt(G0[4], 16); //Tempo lendemain et jour STGE
            tempo =Math.floor(tempo/4) ; //Tempo lendemain uniquement
            idx=-2;
            var txtJ = "";
            if (tempo>0){
              idx = tempo;
              txtJ = "Tempo<br>J+1";
            }
            GID('couleurTarif_J1').style.backgroundColor= couleur[idx+2];
            GID('couleurTarif_J1').innerHTML =txtJ;
            
          if (groupes.length==4) { // La source_data des données est de type UxIx2 ou on est en shelly monophas avec un deuxièeme canal
            GID('PwS_T').innerHTML = LaVal(G2[0]); //Triac
            GID('PwI_T').innerHTML = LaVal(G2[1]); //Triac
            GID('PVAS_T').innerHTML = LaVal(G2[2]); //Triac
            GID('PVAI_T').innerHTML = LaVal(G2[3]); //Triac
            GID('EAJS_T').innerHTML = LaVal(G2[4]);
            GID('EAJI_T').innerHTML = LaVal(G2[5]);      
            GID('EAS_T').innerHTML = LaVal(G2[6]);
            GID('EAI_T').innerHTML = LaVal(G2[7]); 
            tabPW2sT.shift(); //Enleve Pw Triav
            tabPW2sT.shift(); //Enleve PVA
            tabPW2sT.push(parseFloat(G2[0]-G2[1]));
            tabPW2sT.push(parseFloat(G2[2]-G2[3]));
            Plot('SVG_PW2sT',tabPW2sT,'#f44','Puissance Active '+GID("nomSondeFixe").innerHTML+' sur 10 mn en W','aqua','Puissance Apparente sur 10 mn en VA'); 
            if (parseInt(G2[5])==0 && Source!="ShellyEm")  { //Il n'y a pas d'injecté normalement
              GID('produite').innerHTML='';
              GID('PwI_T').innerHTML='';
              GID('PVAI_T').innerHTML='';
              GID('EAJI_T').innerHTML='';
              GID('EAI_T').innerHTML='';
            }
            biSonde=true;
          } else{
            biSonde=false;
          } 
          if (Source_data=='SmartG') { GID('ligneVA').style='display:none;';}   
          GID('LED').style='display:none;';
          setTimeout('LoadData();',2000);
        }
        
      };
      xhttp.open('GET', 'ajax_data', true);
      xhttp.send();
    }
    
    function LoadHisto10mn() {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() { 
        if (this.readyState == 4 && this.status == 200) {
          var retour=this.responseText;
          var groupes=retour.split(GS);
          tabPW2sM.splice(0,tabPW2sM.length);
          tabPW2sM=groupes[1].split(',');
          tabPW2sM.pop();
          Plot('SVG_PW2sM',tabPW2sM,'#f44','Puissance Active '+GID("nomSondeMobile").innerHTML+' sur 10 mn en W','aqua','Puissance Apparente sur 10 mn en VA');
          if (biSonde){
            tabPW2sT.splice(0,tabPW2sT.length);
            tabPW2sT=groupes[2].split(',');
            tabPW2sT.pop();
            GID('SVG_PW2sT').style.display="block";
            Plot('SVG_PW2sT',tabPW2sT,'#f44','Puissance Active '+GID("nomSondeFixe").innerHTML+' sur 10 mn en W','aqua','Puissance Apparente sur 10 mn en VA');
          }
          LoadHisto1an();
        }
        
      };
      xhttp.open('GET', 'ajax_data10mn', true);
      xhttp.send();
    }
    function LoadHisto48h() {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() { 
        if (this.readyState == 4 && this.status == 200) {
          var retour=this.responseText;
          var groupes=retour.split(GS);
          var tabPWM=groupes[1].split(',');
          tabPWM.pop();
          Plot('SVG_PW48hM',tabPWM,'#f33','Puissance Active '+GID("nomSondeMobile").innerHTML+' sur 48h en W','','');
          if (biSonde){
            var tabPWT=groupes[2].split(',');
            tabPWT.pop();
            GID('SVG_PW48hT').style.display="block";
            Plot('SVG_PW48hT',tabPWT,'#f33','Puissance Active '+GID("nomSondeFixe").innerHTML+' sur 48h en W','',''); 
          }
          if (parseFloat(groupes[3])> -100) {
             var tabTemperature=groupes[4].split(',');
            tabTemperature.pop();
            GID('SVG_Temp48h').style.display="block";
            Plot('SVG_Temp48h',tabTemperature,'#3f3',nomTemperature+' sur 48h ','',''); 
          }
          setTimeout('LoadHisto48h();',300000);
        }
        
      };
      xhttp.open('GET', 'ajax_histo48h', true);
      xhttp.send();
    }
    function LoadHisto1an() {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() { 
        if (this.readyState == 4 && this.status == 200) {
          var retour=this.responseText;
          var tabWh=retour.split(',');
          tabWh.pop();
          
          Plot('SVG_Wh1an',tabWh,'#ff4','Energie Active Wh / Jour sur 1an','','');
          LoadHisto48h();
        }
        
      };
      xhttp.open('GET', 'ajax_histo1an', true);
      xhttp.send();
    }
    function Plot(SVG,Tab,couleur1,titre1,couleur2,titre2){
        var Vmax=0;
        var Vmin=0;
        var TabX=[];
        var TabY0=[];
        var TabY1=[];
        for (var i = 0; i < Tab.length; i++) {
              Vmax = Math.max(Math.abs(Tab[i]), Vmax);       
        }    
        var cadrageMax=1;
        var cadrage1=1000000;
        var cadrage2=[10,8,5,4,2,1];
        for (var m=0;m<7;m++){
          for (var i=0;i<cadrage2.length;i++){
              var X=cadrage1*cadrage2[i];
              if ((Vmax)<=X) cadrageMax=X;
          }
          cadrage1=cadrage1/10;
        }
       
        var dX=900/Tab.length;
        const d = new Date();
        var dI=1;
        var label='heure';
        var pixelTic=72;
        var dTextTic=4;
        var moduloText=24;
        var H0=d.getHours()+d.getMinutes()/60;
        var H00= 4*Math.floor(H0/4);
        var X0=18*(H00-H0);
        var Y0=250;
        var Yamp=230;
        var dy=2;
        switch (SVG){
          case  'SVG_PW48hM':
            
          break;
          case  'SVG_PW48hT':
            
          break;
          case 'SVG_Temp48h':
            Y0=450;
            Yamp=430;
            dy=1;
          break;
          case  'SVG_PW2sM':
            label='mn';
            pixelTic=90;
            X0=0;
            dTextTic=1;
            moduloText=-100;
            H00= 0;
            dI=2; //2 courbes PW et PVA
          break;
          case  'SVG_PW2sT':
            label='mn';
            pixelTic=90;
            X0=0;
            dTextTic=1;
            moduloText=-100;
            H00= 0;
            dI=2; //2 courbes PW et PVA
          break;
          case  'SVG_Wh1an':
            label='Mois';
            pixelTic=dX*30.4375;//Mois moyen
            var dTextTic=1;
            moduloText=12;
            H00= d.getMonth();
            X0=dX*(1-d.getDate());
            var Mois=['Jan','Fev','Mars','Avril','Mai','Juin','Juil','Ao&ucirc;t','Sept','Oct','Nov','Dec'];
          break;
          
        }
        var c1='"' + couleur1 + '"';
        var c2='"' + couleur2 + '"';
        var S= "<svg viewbox='0 0 1030 500' height='500' width='100%' id='S_" + SVG +"' onmouseover ='DispVal(this,event," +c1+","+c2+");' >";
        S += "<line x1='100' y1='20' x2='100' y2='480' style='stroke:white;stroke-width:2' />";
        S += "<line x1='100' y1='" + Y0 + "' x2='1000' y2='" + Y0 + "' style='stroke:white;stroke-width:2' />";
        
        for (var x=1000+X0;x>100;x=x-pixelTic){
          var X=x;
          var Y2=Y0+6;
          S +="<line x1='"+X+"' y1='" + Y0 + "' x2='"+X+"' y2='" + Y2 + "' style='stroke:white;stroke-width:2' />";
          X=X-8;
          Y2=Y0+22;
          if (SVG=='SVG_Wh1an') {
            X=X+8;
            S +="<text x='"+X+"' y='" + Y2 + "' style='font-size:16px;fill:white;'>"+Mois[H00]+"</text>";
          }else{
            S +="<text x='"+X+"' y='" + Y2 + "' style='font-size:16px;fill:white;'>"+H00+"</text>";
          }
          H00=(H00-dTextTic+moduloText)%moduloText;
        }
        Y2=Y0-3;
        S +="<text x='980' y='" + Y2 + "' style='font-size:14px;fill:white;'>"+label+"</text>";
        for (var y=-10 ;y<=10;y=y+dy){
          
          Y2=Y0-Yamp*y/10;
          if (Y2<=480){
            S +="<line x1='100' y1='"+Y2+"' x2='1000' y2='"+Y2+"' style='stroke:white;stroke-width:1;stroke-dasharray:2 10;' />";
            Y2=Y2+7;
            var T=cadrageMax*y/10;T=T.toString();
            var X=90-9*T.length;
            S +="<text x='"+X+"' y='"+Y2+"' style='font-size:16px;fill:white;'>"+T+"</text>";
          }
        }
        if (dI==2 && Source_data!='SmartG'){ //Pas de puissance apparente pour SmartG
          S +="<text x='450' y='40' style='font-size:18px;fill:"+couleur2+";'>"+titre2+"</text>";
          S += "<polyline points='"; 
            var j=0;       
            for (var i = 1; i < Tab.length; i = i+dI) {
              var Y = Y0 - Yamp * Tab[i] / cadrageMax;
              var X = 100+dX * i;
              S += X + "," + Y + " ";
              TabX[j]=X;
              TabY1[j]=parseFloat(Tab[i]);
              j++;
            }
          S += "' style='fill:none;stroke:"+couleur2+";stroke-width:4' />";
        }
        S +="<text x='450' y='18' style='font-size:18px;fill:"+couleur1+";'>"+titre1+"</text>";
        S += "<polyline points='";   
          var j=0;     
          for (var i = 0; i < Tab.length; i = i+dI) {
            var Y = Y0 - Yamp * Tab[i] / cadrageMax;
            var X = 100+dX * i;
            S += X + "," + Y + " ";
            TabX[j]=X;
            TabY0[j]=parseFloat(Tab[i]);
            j++;
          }
        S += "' style='fill:none;stroke:"+couleur1+";stroke-width:4' />";
        
        S += "</svg>";
        GID(SVG).innerHTML = S;
        TableauX["S_" + SVG] = TabX; //Sauvegarde valeurs
        TableauY0["S_" + SVG] = TabY0; //Sauvegarde valeurs
        TableauY1["S_" + SVG] = TabY1; //Sauvegarde valeurs
    }
    function DispVal(t,evt,couleur1,couleur2){
      var ClientRect =  t.getBoundingClientRect();
      var largeur_svg=ClientRect.right-ClientRect.left-20; //20 pixels de marge
      var x= Math.round(evt.clientX - ClientRect.left-10);
      x=x*1030/largeur_svg;
      if(x>=0 && x<=1000){
        var p=-1;
        var distM=10000;
        for (var i=0;i<TableauX[t.id].length;i++){ //Recherche position dans tableau valeurs
          var Dist=Math.abs(TableauX[t.id][i]-x);
          if (Dist<=distM) {
            p=i;
            distM=Dist;
          }
          if (Dist==0) i=10000;
        }
        if (p>=0){
          var S="<div style='color:"+couleur1 + ";'>" + TableauY0[t.id][p] + "</div>";
          if (TableauY1[t.id].length>0) S ="<div style='color:"+couleur2 + ";'>"+ TableauY1[t.id][p]+ "</div>" + S;
          x = evt.pageX+10;
          GID("info").style.left=x + "px";
          x = evt.pageY+10;
          GID("info").style.top=x +"px";
          GH("info",S);
          GID("info").style.display="block";
          if (myTimeout !=null) clearTimeout(myTimeout);
          myTimeout=setTimeout(stopAffiche, 5000);
        }
      }
    }
    function stopAffiche(){
      GID("info").style.display="none";
    }
    function EtatActions() {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() { 
        if (this.readyState == 4 && this.status == 200) {
          var retour=this.responseText;
          var message=retour.split(GS);
          
          Source_data=message[1];
          var T="";
          if(message[0]>-100){
               var Temper=parseFloat(message[0]).toFixed(1);
               T="<tr class='temper'><td>" + nomTemperature +"</td><td class='centrer'>"+Temper+"°C</td></tr>";
          }
          var S="";
          if (message[3]>0){ //Nb Actions            
            for (var i=0;i<message[3];i++){ 
              var data=message[i+4].split(RS);
              S+="<tr><td>"+data[1]+"</td>";
              if (data[2]=="On" || data[2]=="Off"){
                S+="<td><div ><div class='centrer'>"+data[2]+"</div></td></tr>";
              } else {
                var W=1+1.99*data[2];
                S+="<td><div class='jaugeBack'><div class='jauge' style='width:"+W+"px'></div><div class='centrer w100'>"+data[2]+"%</div></div></td></tr>";
              }
            }
          }
          S=S+T;
          if (S!=""){
            S="<div><div class='tableau'><table >" +S;
            S +="</table>";
            GH("etatActions",S);
            if(Source=="Ext" ){
                GID("donneeLocale").style.display="block";   
            }
          }
          setTimeout('EtatActions();',3500);
        }
        
      };
      xhttp.open('GET', 'ajax_etatActions', true);
      xhttp.send();
    }
    
    function LaVal(d){
        d=parseInt(d);
        d='           '+d.toString();
        return d.substr(-9,3)+' '+d.substr(-6,3)+' '+d.substr(-3,3);
    }
    
    function AdaptationSource(){
      var d='none';
      if(biSonde){
        d="table-cell";
      }
      const collection = document.getElementsByClassName('dispT');
      for (let i = 0; i < collection.length; i++) {
        collection[i].style.display = d;
      } 
      
      var S='Source : ' 
      if(Source=="Ext"){  
        S +='ESP distant '+int2ip(RMSextIP);
        GID("donneeDistante").style.display="block";
      }else {
        S +='ESP local';
      }
      GH('source',S);
    }
)====";
