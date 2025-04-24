# Automação de telescópio para acompanhamento de corpos celestes com dados vindos do Stellarium
Neste projeto, um telescópio manual na configuração Azimutal foi automatizado para que fosse possível ajustar os ângulos de azimute e altitude a partir de dados do software Stellarium. Abaixo, é possível ver o sistema embarcado no telescópio.

 <p>
  <img src="https://raw.githubusercontent.com/Penguin-Lab/telescopio_stellarium/main/images/telescopio_7.jpeg" height="500" align="left" hspace="20" />
</p>

<p>
  <img src="https://raw.githubusercontent.com/Penguin-Lab/telescopio_stellarium/main/images/telescopio_0.jpeg" height="245" /><br>
  <img src="https://raw.githubusercontent.com/Penguin-Lab/telescopio_stellarium/main/images/telescopio_6.jpeg" height="245" />
</p>

# Estrutura física
A estrutura física da automação do telescópio é composta por três partes: a engrenagem e motor para controle do ângulo de altitude; a engrenagem e motor para controle do ângulo de azimute; e a caixinha com a placa de circuito impresso com a ESP32, que recebe os dados do Stellarium por Wi-fi e comanda os motores. Os dois motores de passo da estrutura são comandados pela ESP32 de 38 pinos por meio de um driver A4988. Todas as peças são projetadas em 3D e foram impressas em PLA, podendo ser encontradas na pasta pecas_3d deste projeto.

<img src="https://raw.githubusercontent.com/Penguin-Lab/telescopio_stellarium/main/images/telescopio_1.jpeg" height="245">
<img src="https://raw.githubusercontent.com/Penguin-Lab/telescopio_stellarium/main/images/telescopio_2.jpeg" height="245">
<img src="https://raw.githubusercontent.com/Penguin-Lab/telescopio_stellarium/main/images/telescopio_4.jpeg" height="245">
<img src="https://raw.githubusercontent.com/Penguin-Lab/telescopio_stellarium/main/images/telescopio_5.jpeg" height="245">

Para iniciar o sistema, o telescópio, atualmente, deve ser posicionado virado para o Norte e seus ângulos (mostrados nos marcadores de papel) devem estar em 0°. Depois disso, deve-se enviar o código para a ESP32 com o IP correto do computador que está rodando o servidor do Stellarium e então liga-se o sistema por meio e uma chave abaixo da caixinha. O telescópio é alimentado por uma fonte de 12V.

# Software
Na pasta de códigos, há o código de programação da ESP32 (38 pinos). Este é um exemplo em C++ com multitask para o comando dos motores a partir dos dados do Stelarium. A ESP32 receberá as informações de altitude e azimute de um corpo celeste que o usuário do Stellarium clicou sobre.

# Configuração do Stellarium
Para esta solução proposta, foi utilizada a [API do Stellarium](https://stellarium.org/doc/23.0/remoteControlApi.html) em que os dados de corpos celestes são recebidos por meio de uma requisição HTTP. Vamos ver os passos para configurar o Stellarium:
* Instale o [Stellarium](https://stellarium.org/)
* Instale o plugin RemoteControl:
  - Com o Stellarium aberto, ao pressionar a tecla F2, a janela de configuração é aberta. Nesse momento, deve-se selecionar a aba de plugins. Já na aba de plugins, seleciona-se o RemoteControl. Após selecionar o plugin desejado, marque a opção "Carregar ao iniciar". Agora, feche a janela e feche o programa.
    <p><img src="https://github.com/Penguin-Lab/telescopio_stellarium/blob/main/images/stellarium_0.png" height="400"></p>
  - Ao reiniciar o Stellarium, entrando novamente na página do plugin RemoteControl, o botão "configurar" se torna disponível na interface do programa, sendo possível ajustar os parâmetros de acordo com a figura abaixo. Marque os dois pontos acima da janela, como mostra a figura e clique em Salvar Configurações. Agora, pode-se fechar a janela e fechar o programa.
    <p><img src="https://github.com/Penguin-Lab/telescopio_stellarium/blob/main/images/stellarium_1.png" height="400"></p>
  - Agora, toda vez que abrir o Stellarium, ele funcionará como um servidor de acesso aos dados correntes dos corpos celestes apresentados por ele. Para que esses dados estejam relacionados corretamente com a sua localização geográfica, no menu do canto inferior esquerdo, abra a Janela de Localização (F6), pesquise o seu local para que no canto inferior apresente a sua posição geográfica corretamente.
  - Pronto! Basta agora clicar nos astros do Stellarium que a ESP32 já estará consumindo esses dados.

# Funcionamento da API do RemoteControl
A [API do RemoteControl do Stellarium](https://stellarium.org/doc/23.0/remoteControlApi.html) funciona na base de requisições HTTP. Ou seja, é possível usar a ESP32 ou qualquer linguagem que consiga realizar uma operação GET.HTTP para uma URL específica. Abaixo, é possível ver um exemplo do próprio site da API. Caso se queira receber dados atuais sobre a Lua, basta pedir um GET.HTTP para o link:

_http://localhost:8090/api/objects/info?name=moon&format=json_

ou

_http://localhost:8090/api/objects/info?name=lua&format=json_

Se jogar esse link no browse, é possível conferir o resultado desta requisição HTTP:

**{"above-horizon":false,"age":"26.16","airmass":-1,"albedo":0.11999999731779099,"altitude":-43.54344803507313,"altitude-geometric":-43.54344803507313,"ambientInt":0.054999977350234985,"ambientLum":0.0027867841999977827,"appSidTm":"8h50m29.5s","appSidTm-dd":132.6229148734547,"azimuth":245.27595169840848,"azimuth-geometric":245.27595169840848,"central_b":0.34271443620666603,"central_l":5.4017087382406395,"colongitude":90.04393694182852,"dec":-2.318601324208655,"decJ2000":-2.4583903909500355,"distance":0.0024639369185977277,"distance-km":368599.7165391639,"ecl-elongation":-0.7243564464838901,"ecl-elongation-deg":"-41.5026°","ecl-elongation-dms":"-41°30'09\"","ecliptic-obliquity":23.43598624024554,"elat":0.31553450590723886,"elatJ2000":0.31594411966903047,"elong":353.42953041853946,"elongJ2000":353.0758971843355,"elongation":0.7244721712882872,"elongation-deg":"41.5092°","elongation-dms":"+41°30'33\"","found":true,"glat":-59.15317236039611,"glong":82.73947535635295,"heliocentric-distance":1.00408417868352,"heliocentric-distance-km":150208855.12557617,"heliocentric-velocity":"[0.00952658, -0.0136085, 5.71737e-05]","heliocentric-velocity-kms":"28.76249","hourAngle-dd":138.7804304700692,"hourAngle-hms":"9h15m07.3s","iauConstellation":"Psc","illumination":12.61147403717041,"is-waning":true,"libration_b":0.34271443620666603,"libration_l":-5.4017087382406395,"localized-name":"Lua","meanSidTm":"8h50m29.5s","meanSidTm-dd":132.62288401855767,"name":"Moon","object-type":"moon","pa_axis":338.204304094815,"parallacticAngle":121.72861493520773,"penumbral-eclipse-magnitude":0,"phase":0.12611474096775055,"phase-angle":2.4154941734967172,"phase-angle-deg":"138.3976°","phase-angle-dms":"+138°23'51\"","phase-name":"Lua minguante","ra":-6.15751559661449,"raJ2000":-6.482643384375062,"rise":"2h26m","rise-dhr":2.4252777777777776,"scale":1,"set":"15h06m","set-dhr":15.093611111111112,"sglat":18.72705971457184,"sglong":288.2637687915038,"size":0.009426958597546628,"size-dd":0.5401249412839874,"size-deg":"0.54012°","size-dms":"+0°32'24.45\"","subsolar_b":0.9546297201113992,"subsolar_l":144.23647163485037,"transit":"8h48m","transit-dhr":8.806666666666667,"type":"Planet","umbral-eclipse-magnitude":0,"velocity":"[3.94235e-05, 0.000619631, 5.63846e-05]","velocity-kms":"1.07946","vmag":-7.989984035491943,"vmage":-7.780420780181885}**

A ESP32 pega exatamente a informação que se deseja ao se utilizar uma biblioteca de JSON para se acessar exatamente a informação. Por exemplo, caso se queira pegar o ângulo de azimute, pode-se utilizar as seguintes linhas de código (com a biblioteca de JSON: Arduino_JSON.h):

<p>
JSONVar info_astro = JSON.parse(payload);
</p>
<p>
az = info_astro["azimuth"];
</p>

# Referências
<p> [Stellarium](https://stellarium.org/) </p>
<p> [API do Stellarium](https://stellarium.org/doc/23.0/remoteControlApi.html) </p>
<p> [Explicação sobre como configurar o RemoteControl do Stellarium](https://devgirlunderscore.medium.com/how-to-use-stellarium-http-api-f0942d2c8820) </p>
