// https://twitter.com/wakwak-koba/

//#define RADIKO_USER "SET YOUR MAIL-ADDRESS"
//#define RADIKO_PASS "SET YOUR PREMIUM PASS"

#define I2S_LRC  39
#define I2S_BCLK 5
#define I2S_DOUT 38

#include <WiFi.h>
#include <WebRadio_Radiko.h>
#include <AudioOutputI2S.h>
#include <ArduinoOTA.h>
#include <WebServer.h>
#include <ArduinoJson.h>

static AudioOutputI2S out(0, AudioOutputI2S::EXTERNAL_I2S, 10);
static Radiko radio(&out, APP_CPU_NUM);
static WebServer httpd(80);

static const char INDEX_HTML[] PROGMEM = R"HTML(
<!doctype html>
<html lang="ja">
  <head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>TinyRadi</title>
    <link rel="icon" href="data:," />
    <meta name="color-scheme" content="light dark" />
    <script type="module">
(function(){let e=document.createElement(`link`).relList;if(e&&e.supports&&e.supports(`modulepreload`))return;for(let e of document.querySelectorAll(`link[rel="modulepreload"]`))n(e);new MutationObserver(e=>{for(let t of e)if(t.type===`childList`)for(let e of t.addedNodes)e.tagName===`LINK`&&e.rel===`modulepreload`&&n(e)}).observe(document,{childList:!0,subtree:!0});function t(e){let t={};return e.integrity&&(t.integrity=e.integrity),e.referrerPolicy&&(t.referrerPolicy=e.referrerPolicy),e.crossOrigin===`use-credentials`?t.credentials=`include`:e.crossOrigin===`anonymous`?t.credentials=`omit`:t.credentials=`same-origin`,t}function n(e){if(e.ep)return;e.ep=!0;let n=t(e);fetch(e.href,n)}})();var e=new class{parseXml=e=>new DOMParser().parseFromString(e,`application/xml`);getDate=e=>{let t=Number(e.slice(0,4)),n=Number(e.slice(4,6))-1,r=Number(e.slice(6,8)),i=Number(e.slice(8,10)),a=Number(e.slice(10,12));return new Date(t,n,r,i,a)};formatTime=e=>e.toLocaleTimeString([],{hour:`2-digit`,minute:`2-digit`});info=()=>fetch(`/api/info`).then(e=>e.json()).then(e=>e.info);play=e=>fetch(`/api/play`,{method:`POST`,headers:{"Content-Type":`application/json`},body:JSON.stringify({station:e})}).then(e=>e.json());stop=()=>fetch(`/api/stop`,{method:`POST`}).then(e=>e.json());nowPrograms=e=>fetch(`https://api.radiko.jp/program/v3/now/${e}.xml`).then(e=>e.text()).then(e=>{let t=this.parseXml(e);return Array.from(t.querySelectorAll(`station`)).map(e=>{let t={id:e.getAttribute(`id`),name:e.querySelector(`name`)?.textContent||null},n=Array.from(e.querySelectorAll(`prog`)).map(e=>{let t={id:e.getAttribute(`id`),time_raw:{ft:e.getAttribute(`ft`),to:e.getAttribute(`to`),ftl:e.getAttribute(`ftl`),tol:e.getAttribute(`tol`),dur:e.getAttribute(`dur`)},time:{ft:e.getAttribute(`ft`)?this.getDate(e.getAttribute(`ft`)):null,to:e.getAttribute(`to`)?this.getDate(e.getAttribute(`to`)):null,ftl:e.getAttribute(`ftl`)?this.getDate(e.getAttribute(`ftl`)):null,tol:e.getAttribute(`tol`)?this.getDate(e.getAttribute(`tol`)):null,dur:e.getAttribute(`dur`)?Number(e.getAttribute(`dur`)):null,formatted:null},title:e.querySelector(`title`)?.textContent||null,url:e.querySelector(`url`)?.textContent||null,desc:e.querySelector(`desc`)?.textContent||null,info:e.querySelector(`info`)?.textContent||null,pfm:e.querySelector(`pfm`)?.textContent||null,img:e.querySelector(`img`)?.textContent||null};return t.time.ft&&t.time.to&&(t.time.formatted=`${this.formatTime(t.time.ft)} - ${this.formatTime(t.time.to)}`),t});return{...t,programs:n}})})},t=document.querySelector(`#app`);if(!t)throw Error(`app root not found`);t.innerHTML=`
  <main>
    <header>
      <h1>TinyRadi</h1>
      <p id="area"></p>
    </header>
    <div id="now-playing">
      <p><img id="media-image" width="480" height="300" alt="画像なし"></p>
      <div>
        <h2 id="media-title">再生停止中</h2>
        <p id="media-station"></p>
        <p id="media-pfm"></p>
        <p id="media-time"></p>
      </div>
    </div>
    <ul id="panel"></ul>
    <button id="open-details">番組詳細</button>
    <dialog id="details-dialog">
      <div id="details"></div>
      <button id="close-details">閉じる</button>
    </dialog>
  </main>
`;var n=document.querySelector(`#area`),r=document.querySelector(`#media-image`),i=document.querySelector(`#media-title`),a=document.querySelector(`#media-station`),o=document.querySelector(`#media-pfm`),s=document.querySelector(`#media-time`),c=document.querySelector(`#panel`),l=document.querySelector(`#open-details`),u=document.querySelector(`#details-dialog`),d=document.querySelector(`#details`),f=document.querySelector(`#close-details`);if(!n||!r||!i||!a||!o||!s||!c||!l||!u||!d||!f)throw Error(`required elements not found`);var p=await e.info(),{id:m=``,name:h=``}=p.area??{},g=p.stationId,_=e=>{let t=Date.now();for(let n=0;n<e.length;n+=1){let r=e[n];if(!r.time.to||t<r.time.to.getTime())return r}return e.length>0?e[e.length-1]:null},v=[],y=async t=>{v=await e.nowPrograms(t)},b=(e=!1)=>{let t=e=>{let{id:t,name:n}=e,{id:r,title:i,pfm:a,time:{formatted:o}={}}=_(e.programs)||{};return`
        <button value="${t??``}" data-program-id="${r??``}" ${g===t?`class="playing"`:``}>
          <h2>
            <div class="title" title="${i??`タイトルなし`}">
              <span>${i??`タイトルなし`}</span>
            </div>
            <div class="station-name" title="${n??`不明な放送局`}">
              <span>${n??`不明な放送局`}</span>
            </div>
          </h2>
          <p class="pfm"><span title="${a??``}">${a??``}</span></p>
          <p class="time"><span title="${o??``}">${o??``}</span></p>
        </button>`};e?c.innerHTML=v.map(e=>`
      <li>
        ${t(e)}
      </li>`).concat([`<li><button value="" id="stop"><h2>停止</h2></button></li>`]).join(``):v.forEach(e=>{let n=e.id,r=c.querySelector(`button[value="${n}"]`);if(!r)return;let i=_(e.programs);r.dataset.programId!==i?.id&&(r.outerHTML=t(e))})},x=null,S=()=>{let e=g&&v.find(e=>e.id===g)||null,t=e?_(e.programs):null;if(!g||!e||!t){let n=x!==null;return x=null,{station:e,program:t,changed:n,isEmpty:!0}}let n=x!==t.id;return x=t.id,{station:e,program:t,changed:n,isEmpty:!1}},C=({station:e,program:t,changed:n,isEmpty:r})=>{if(!(`mediaSession`in navigator)||!n)return;if(r||!e||!t){navigator.mediaSession.metadata=null;return}let{name:i}=e,{title:a,img:o}=t;navigator.mediaSession.metadata=new MediaMetadata({title:a??``,artist:i??``,artwork:[{src:o??``,sizes:`480x300`}]})},w=({station:e,program:t,changed:n,isEmpty:c})=>{if(n){if(c||!e||!t){r.src=``,r.alt=`画像なし`,i.textContent=`再生停止中`,a.textContent=``,o.textContent=``,s.textContent=``;return}r.src=t.img??``,r.alt=t.title??`画像なし`,i.textContent=t.title??``,a.textContent=e.name??``,o.textContent=t.pfm??``,s.textContent=t.time.formatted??``}},T=({station:e,program:t,changed:n,isEmpty:r},i=!1)=>{if(!(!u.open||u.open&&!i&&!n)){if(r||!e||!t){d.innerHTML=``;return}d.innerHTML=((e,t)=>{let{title:n,time:{formatted:r}={},desc:i,info:a,pfm:o,url:s,img:c}=t;return`
      <p><img src="${c}" alt="${n??`タイトルなし`}"></p> 
      <h2>${n??`タイトルなし`}</h2>
      <p>${e??`不明な放送局`}</p>
      <p>${r??`放送時間不明`}</p>
      ${i||a?`<p>${i??``}${a??``}</p>`:``}
      ${o?`<p>${o}</p>`:``}
      ${s?`<p>番組Webサイト: <a href="${s}" target="_blank">${s}</a></p>`:``}
    `})(e.name,t),d.querySelectorAll(`a`).forEach(e=>{e.target=`_blank`}),x=t.id}},E=async()=>{try{if(!m){alert(`現在地の検出に失敗しました`);return}else if(m===`OUT`){alert(`サービス提供エリア外のためTinyRadiを利用できません`);return}n.textContent=h??``;let e=()=>{y(m).then(()=>{b();let e=S();C(e),w(e),T(e),r()})},t=null,r=()=>{t!==null&&(window.clearTimeout(t),t=null);let n=v.map(e=>_(e.programs)).filter(e=>e!==null).reduce((e,t)=>e===null||t.time.to&&e.time.to&&t.time.to.getTime()<e.time.to.getTime()?t:e,null);if(n===null||n.time.to===null)return;let r=Math.max(n.time.to.getTime()-Date.now(),3e4);t=window.setTimeout(()=>{e()},r)};y(m).then(()=>{b(!0);let e=S();C(e),w(e),T(e),r()})}catch(e){alert(`初期化に失敗しました: ${String(e)}`)}},D=async t=>{g=t,g?await e.play(g):await e.stop(),c.querySelectorAll(`button`).forEach(e=>{let t=g!==``&&e.value===g;e.classList.toggle(`playing`,t);let n=e.querySelector(`.status`);n&&(n.textContent=t?`再生中`:``)});let n=S();C(n),w(n),T(n)};c.addEventListener(`click`,async e=>{let t=e.target instanceof HTMLElement?e.target.closest(`button`):null;t&&await D(t.value)});var O=e=>{let t=(v.findIndex(e=>e.id===p.stationId)+e+v.length)%v.length;D(v[t].id??``)};l.addEventListener(`click`,()=>{u.showModal(),T(S(),!0)}),f.addEventListener(`click`,()=>{u.close()}),u.addEventListener(`close`,()=>{d.innerHTML=``}),`mediaSession`in navigator&&(navigator.mediaSession.setActionHandler(`nexttrack`,()=>{O(1)}),navigator.mediaSession.setActionHandler(`previoustrack`,()=>{O(-1)})),document.addEventListener(`keydown`,e=>{let t=(t,n)=>{if(e.repeat)return;let r=Array.from(c.querySelectorAll(`button`)),i=r.findIndex(e=>e.value===p.stationId||e.value===``),a=r[(()=>{if(t===`vertical`){let e=getComputedStyle(c),t={rows:e.getPropertyValue(`grid-template-rows`),columns:e.getPropertyValue(`grid-template-columns`)},a={rows:t.rows.split(` `).length,columns:t.columns.split(` `).length},o=a.rows*a.columns,s=(i+n*a.columns+o)%o;for(;s>r.length-1;)s=(s+n*a.columns+o)%o;return s}else if(t===`horizontal`)return(i+n+r.length)%r.length;return i})()];a.focus(),D(a.value)};switch(e.key){case`ArrowUp`:t(`vertical`,-1);break;case`ArrowDown`:t(`vertical`,1);break;case`ArrowLeft`:t(`horizontal`,-1);break;case`ArrowRight`:t(`horizontal`,1);break;case`Enter`:e.repeat&&e.preventDefault();break}}),E();
    </script>
    <style>
img{max-width:100%}header{align-items:center;gap:16px;margin-block:1.33em;display:flex}header>*{margin:0}#now-playing{gap:16px;min-height:200px;display:flex}#now-playing>p{flex-shrink:0}#now-playing>div{flex-grow:1}@media (width<480px){#now-playing{min-height:340px;display:block}}#panel{grid-template-columns:repeat(auto-fit,minmax(240px,1fr));grid-auto-rows:100px;place-content:center;gap:8px;padding-inline-start:0;list-style:none;display:grid}#panel button{text-align:left;cursor:pointer;border:none;border-radius:4px;flex-direction:column;width:100%;height:100%;padding:8px;display:flex;overflow:hidden}#panel button.playing{color:#fff;background-color:#4c7faf}#panel button#stop{color:#fff;background-color:#af4c4c}#panel button>*{margin:0}#panel button span{white-space:nowrap;text-overflow:ellipsis;overflow:hidden}#panel button h2{font-size:1.2em}#panel button div,#panel button p{align-items:center;display:flex}#panel button .title{height:24px}#panel button .station-name{height:20px;font-size:.8em}#panel button .time{align-self:flex-end;margin-block-start:auto;font-size:.8em}#media-image{width:240px;height:auto;display:block}#media-title{font-size:1.5em;font-weight:700}#media-station{font-weight:700}#media-time{font-size:.8em}#details-dialog{scrollbar-gutter:stable both-edges}#details{max-width:480px}#close-details{width:100%}@media (width<768px){footer{font-size:.8em}}
    </style>
  </head>
  <body>
    <div id="app"></div>
  </body>
</html>
)HTML";

void setup() {
  Serial.begin(115200);

#ifdef CONFIG_IDF_TARGET_ESP32S3
  out.SetPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);    // bck, lrc, dout
  // pinMode( 4, OUTPUT); digitalWrite( 4, LOW);   // XSMT
  // pinMode( 5, OUTPUT); digitalWrite( 5, LOW);   // FMT
  // pinMode(16, OUTPUT); digitalWrite(16, LOW);   // SCK
  // pinMode(17, OUTPUT); digitalWrite(17, LOW);   // FLT
  // pinMode(18, OUTPUT); digitalWrite(18, LOW);   // DEMP
#else
//out.SetPinout(26, 25, 22);  // bck, lrc, dout
#endif

  WiFi.begin();
  for(int count = 0; WiFi.status() != WL_CONNECTED; count++) {
    if(count > 300)
      ESP.restart();
    Serial.print(".");
    delay(100);
  }
  Serial.print("IP address:");
  Serial.println(WiFi.localIP());  

  ArduinoOTA.onStart([]() {
    radio.stop();
    out.stop();
  });
  ArduinoOTA.begin();

#if defined( RADIKO_USER ) && defined( RADIKO_PASS )
  radio.setAuthorization(RADIKO_USER, RADIKO_PASS);
#endif
  // radio.setEnableSBR(true);
  radio.onPlay = [](const char * id, const size_t station_idx) {
    Serial.printf("onPlay:%s\n", id);
  };
  radio.onInfo = [](const char * text) {
    Serial.println(text);
  };
  radio.setSyslog("255.255.255.255");
  
  if(!radio.begin()) {
    Serial.println("failed: radio.begin()");
    for(;;);
  }

  httpd.on("/api/info", HTTP_GET, []() {
    JsonDocument response;
    response["status"] = "ok";
    response["info"]["area"]["id"] = radio.getArea();
    response["info"]["area"]["name"] = radio.getAreaName();
    response["info"]["area"]["nameEn"] = radio.getAreaNameEn();
    response["info"]["stationId"] = radio.getStationId();

    sendJson(200, response);
  });

  httpd.on("/api/play", HTTP_POST, []() {
    // ESP32 WebServerでは生のリクエストボディを
    // arg("plain")で取得する
    if (!httpd.hasArg("plain")) {
      sendJsonError(
        400,
        "missing_body",
        "JSON request body is required"
      );
      return;
    }

    JsonDocument request;
    DeserializationError error =
      deserializeJson(request, httpd.arg("plain"));

    if (error) {
      sendJsonError(
        400,
        "invalid_json",
        error.c_str()
      );
      return;
    }

    if (!request["station"].is<const char*>()) {
      sendJsonError(
        400,
        "invalid_station",
        "station must be a char*"
      );
      return;
    }

    const char* stationId = request["station"].as<const char*>();

    radio.setStation(stationId);

    auto station = radio.getStation();

    if (station == nullptr) {
      sendJsonError(
        404,
        "station_not_found",
        "The specified station does not exist"
      );
      return;
    }

    station->play();

    JsonDocument response;
    response["ok"] = true;
    response["stationId"] = stationId;

    sendJson(202, response);
  });

  httpd.on("/api/stop", HTTP_POST, []() {
    radio.stop();

    JsonDocument response;
    response["status"] = "ok";

    sendJson(200, response);
  });

  httpd.on("/", HTTP_GET, []() {
    httpd.send_P(200, "text/html; charset=utf-8", INDEX_HTML);
  });

  httpd.onNotFound([]() {
    if (httpd.uri().startsWith("/api/")) {
      sendJsonError(
        404,
        "endpoint_not_found",
        "API endpoint not found"
      );
      return;
    }

    httpd.send(
      404,
      "text/plain; charset=utf-8",
      "Not Found"
    );
  });

  httpd.begin();
  radio.play();
// #ifdef CONFIG_IDF_TARGET_ESP32S3
//   digitalWrite( 4, HIGH);  // XSMT
// #endif
}

void sendJson(int statusCode, JsonDocument& doc) {
  String body;
  serializeJson(doc, body);

  httpd.send(
    statusCode,
    "application/json; charset=utf-8",
    body
  );
}

void sendJsonError(int statusCode, const char* code, const char* message) {
  JsonDocument doc;

  doc["error"]["code"] = code;
  doc["error"]["message"] = message;

  sendJson(statusCode, doc);
}

void loop() {
  ArduinoOTA.handle();
  httpd.handleClient();
  radio.handle();
}
