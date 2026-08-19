// https://twitter.com/wakwak-koba/

//#define RADIKO_USER "SET YOUR MAIL-ADDRESS"
//#define RADIKO_PASS "SET YOUR PREMIUM PASS"

#define I2S_BCLK D0
#define I2S_DOUT D1
#define I2S_LRC  D2

#include <WiFi.h>
#include "config.h"
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
(function(){let e=document.createElement(`link`).relList;if(e&&e.supports&&e.supports(`modulepreload`))return;for(let e of document.querySelectorAll(`link[rel="modulepreload"]`))n(e);new MutationObserver(e=>{for(let t of e)if(t.type===`childList`)for(let e of t.addedNodes)e.tagName===`LINK`&&e.rel===`modulepreload`&&n(e)}).observe(document,{childList:!0,subtree:!0});function t(e){let t={};return e.integrity&&(t.integrity=e.integrity),e.referrerPolicy&&(t.referrerPolicy=e.referrerPolicy),e.crossOrigin===`use-credentials`?t.credentials=`include`:e.crossOrigin===`anonymous`?t.credentials=`omit`:t.credentials=`same-origin`,t}function n(e){if(e.ep)return;e.ep=!0;let n=t(e);fetch(e.href,n)}})();function e(t){"@babel/helpers - typeof";return e=typeof Symbol==`function`&&typeof Symbol.iterator==`symbol`?function(e){return typeof e}:function(e){return e&&typeof Symbol==`function`&&e.constructor===Symbol&&e!==Symbol.prototype?`symbol`:typeof e},e(t)}function t(t,n){if(e(t)!=`object`||!t)return t;var r=t[Symbol.toPrimitive];if(r!==void 0){var i=r.call(t,n||`default`);if(e(i)!=`object`)return i;throw TypeError(`@@toPrimitive must return a primitive value.`)}return(n===`string`?String:Number)(t)}function n(n){var r=t(n,`string`);return e(r)==`symbol`?r:r+``}function r(e,t,r){return(t=n(t))in e?Object.defineProperty(e,t,{value:r,enumerable:!0,configurable:!0,writable:!0}):e[t]=r,e}var i=new class{constructor(){r(this,`parseXml`,e=>new DOMParser().parseFromString(e,`application/xml`)),r(this,`getDate`,e=>{let t=Number(e.slice(0,4)),n=Number(e.slice(4,6))-1,r=Number(e.slice(6,8)),i=Number(e.slice(8,10)),a=Number(e.slice(10,12));return new Date(t,n,r,i,a)}),r(this,`formatTime`,e=>e.toLocaleTimeString([],{hour:`2-digit`,minute:`2-digit`})),r(this,`info`,()=>fetch(`/api/info`).then(e=>e.json()).then(e=>e.info)),r(this,`play`,e=>fetch(`/api/play`,{method:`POST`,headers:{"Content-Type":`application/json`},body:JSON.stringify({station:e})}).then(e=>e.json())),r(this,`stop`,()=>fetch(`/api/stop`,{method:`POST`}).then(e=>e.json())),r(this,`nowPrograms`,e=>fetch(`https://api.radiko.jp/program/v3/now/${e}.xml`).then(e=>e.text()).then(e=>{let t=this.parseXml(e);return Array.from(t.querySelectorAll(`station`)).map(e=>{var t;let n={id:e.getAttribute(`id`),name:((t=e.querySelector(`name`))==null?void 0:t.textContent)||null},r=Array.from(e.querySelectorAll(`prog`)).map(e=>{var t,n,r,i,a,o;let s={id:e.getAttribute(`id`),time_raw:{ft:e.getAttribute(`ft`),to:e.getAttribute(`to`),ftl:e.getAttribute(`ftl`),tol:e.getAttribute(`tol`),dur:e.getAttribute(`dur`)},time:{ft:e.getAttribute(`ft`)?this.getDate(e.getAttribute(`ft`)):null,to:e.getAttribute(`to`)?this.getDate(e.getAttribute(`to`)):null,ftl:e.getAttribute(`ftl`)?this.getDate(e.getAttribute(`ftl`)):null,tol:e.getAttribute(`tol`)?this.getDate(e.getAttribute(`tol`)):null,dur:e.getAttribute(`dur`)?Number(e.getAttribute(`dur`)):null,formatted:null},title:((t=e.querySelector(`title`))==null?void 0:t.textContent)||null,url:((n=e.querySelector(`url`))==null?void 0:n.textContent)||null,desc:((r=e.querySelector(`desc`))==null?void 0:r.textContent)||null,info:((i=e.querySelector(`info`))==null?void 0:i.textContent)||null,pfm:((a=e.querySelector(`pfm`))==null?void 0:a.textContent)||null,img:((o=e.querySelector(`img`))==null?void 0:o.textContent)||null};return s.time.ft&&s.time.to&&(s.time.formatted=`${this.formatTime(s.time.ft)} - ${this.formatTime(s.time.to)}`),s});return{...n,programs:r}})}))}},a=document.querySelector(`#app`);if(!a)throw Error(`app root not found`);a.innerHTML=`
  <main>
    <header id="header">
      <h1>TinyRadi</h1>
      <p id="area"></p>
    </header>
    <ul id="panel" hidden></ul>
    <div id="now-playing" hidden>
      <p>
        <button id="select-station">選局画面</button>
      </p>
      <p><img id="media-image" width="480" height="300" alt="画像なし"></p>
      <div>
        <h2 id="media-title">再生停止中</h2>
        <p id="media-station"></p>
        <p id="media-pfm"></p>
        <p id="media-time"></p>
      </div>
      <p class="controls">
        <button id="play-stop-button">停止</button>
      </p>
    </div>
  </main>
`;var o=document.querySelector(`#header`),s=document.querySelector(`#area`),c=document.querySelector(`#panel`),l=document.querySelector(`#now-playing`),u=document.querySelector(`#select-station`),d=document.querySelector(`#media-image`),f=document.querySelector(`#media-title`),p=document.querySelector(`#media-station`),m=document.querySelector(`#media-pfm`),h=document.querySelector(`#media-time`),g=document.querySelector(`#play-stop-button`);if(!o||!s||!c||!l||!u||!d||!f||!p||!m||!h||!g)throw Error(`required elements not found`);(async()=>{var e;let t=await i.info(),{id:n=``,name:r=``}=(e=t.area)==null?{}:e,a=t.stationId,_=e=>{let t=Date.now();for(let n=0;n<e.length;n+=1){let r=e[n];if(!r.time.to||t<r.time.to.getTime()+6e4)return r}return e.length>0?e[e.length-1]:null},v=[],y=async e=>{v=await i.nowPrograms(e)},b=(e=!1)=>{let t=e=>{let{id:t,name:n}=e,{id:r,title:i,time:{formatted:o}={}}=_(e.programs)||{};return`
          <button value="${t==null?``:t}" data-program-id="${r==null?``:r}" ${a===t?`class="playing"`:``}>
            <h2>
              <div class="title" title="${i==null?`タイトルなし`:i}">
                ${i==null?`タイトルなし`:i}
              </div>
              <div class="station-name" title="${n==null?`不明な放送局`:n}">
                ${n==null?`不明な放送局`:n}
              </div>
            </h2>
            <p class="time"><span title="${o==null?``:o}">${o==null?``:o}</span></p>
          </button>`};e?c.innerHTML=v.map(e=>`
        <li>
          ${t(e)}
        </li>`).join(``):v.forEach(e=>{let n=e.id,r=c.querySelector(`button[value="${n}"]`);if(!r)return;let i=_(e.programs);r.dataset.programId!==(i==null?void 0:i.id)&&(r.outerHTML=t(e))})},x=a,S=null,C=()=>{let e=x&&v.find(e=>e.id===x)||null,t=e?_(e.programs):null;if(!x||!e||!t){let n=S!==null;return S=null,{station:e,program:t,changed:n,isEmpty:!0}}let n=S!==t.id;return S=t.id,{station:e,program:t,changed:n,isEmpty:!1}},w=({station:e,program:t,changed:n,isEmpty:r})=>{var i,a,o,s,c,l;if(n){if(r||!e||!t){d.src=``,d.alt=`画像なし`,f.textContent=`再生停止中`,p.textContent=``,m.textContent=``,h.textContent=``;return}d.src=(i=t.img)==null?``:i,d.alt=(a=t.title)==null?`画像なし`:a,f.textContent=(o=t.title)==null?``:o,p.textContent=(s=e.name)==null?``:s,m.textContent=(c=t.pfm)==null?``:c,h.textContent=(l=t.time.formatted)==null?``:l}},T=async()=>{try{if(!n){alert(`現在地の検出に失敗しました`);return}else if(n===`OUT`){alert(`サービス提供エリア外のためTinyRadiを利用できません`);return}s.textContent=r==null?``:r;let e=()=>{y(n).then(()=>{b(),w(C()),i()})},t=null,i=()=>{t!==null&&(window.clearTimeout(t),t=null);let n=v.map(e=>_(e.programs)).filter(e=>e!==null).reduce((e,t)=>e===null||t.time.to&&e.time.to&&t.time.to.getTime()<e.time.to.getTime()?t:e,null);if(n===null||n.time.to===null)return;let r=Math.max(n.time.to.getTime()+6e4-Date.now(),3e4);t=window.setTimeout(()=>{e()},r)};y(n).then(()=>{b(!0),w(C()),i()}),a?(o.hidden=c.hidden=!0,l.hidden=!1):(o.hidden=c.hidden=!1,l.hidden=!0)}catch(e){alert(`初期化に失敗しました: ${String(e)}`)}},E=async(e=null)=>{a=e,a?(await i.play(a),x=a):await i.stop(),c.querySelectorAll(`button`).forEach(e=>{let t=e.value===a;e.classList.toggle(`playing`,t)}),w(C())};c.addEventListener(`click`,async e=>{let t=e.target instanceof HTMLElement?e.target.closest(`button`):null;t&&(t.value!==a&&(await E(t.value),g.textContent=`停止`),o.hidden=c.hidden=!0,l.hidden=!1)}),u.addEventListener(`click`,()=>{l.hidden=!0,o.hidden=c.hidden=!1}),g.addEventListener(`click`,async()=>{a===null?(await E(x),g.textContent=`停止`):(await E(null),g.textContent=`再生`)}),T()})();
    </script>
    <style>
header:not([hidden]){align-items:center;gap:16px;margin-top:1.33em;margin-bottom:1.33em;display:flex}header:not([hidden])>*{margin:0}#panel:not([hidden]){grid-template-columns:repeat(auto-fit,minmax(180px,1fr));grid-auto-rows:75px;place-content:center;gap:8px;padding:0;list-style:none;display:grid}#panel:not([hidden]) button{text-align:left;cursor:pointer;border:none;border-radius:4px;width:100%;height:100%;padding:8px;overflow:hidden}#panel:not([hidden]) button.playing{color:#fff;background-color:#4c7faf}#panel:not([hidden]) button#stop{color:#fff;background-color:#af4c4c}#panel:not([hidden]) button#stop h2{height:100%}#panel:not([hidden]) button>*{margin:0}#panel:not([hidden]) button h2{font-size:1.2em}#panel:not([hidden]) button h2 div{white-space:nowrap;text-overflow:ellipsis;overflow:hidden}#panel:not([hidden]) button .title{height:24px}#panel:not([hidden]) button .station-name{height:20px;font-size:.8em}#panel:not([hidden]) button .time{text-align:right;font-size:.8em}#now-playing:not([hidden]){max-width:480px;margin:0 auto}#media-image{max-width:100%;height:auto;display:block}#media-title{font-size:1.5em;font-weight:700}#media-station{font-weight:700}#media-time{font-size:.8em}.controls{text-align:center}
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
  out.SetGain(0.5f);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
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
  radio.setEnableSBR(true);
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

    auto station = radio.getStation(0);

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
    response["stationId"] = stationId;

    sendJson(202, response);
  });

  httpd.on("/api/stop", HTTP_POST, []() {
    radio.stop();

    JsonDocument response;
    response["stationId"] = radio.getStationId();

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
