$(document).ready(function () {
    capturePhoto();
    loadModel();
    copyJSON();
});

var stepCount, currentFetchedMat = 0;
var faceDetectionResult = '';
let currentActive = 1;

function loadModel() {

    var fetchMat = "";
    const { createWorker } = Tesseract;
    const worker = createWorker({
        workerPath: '../node_modules/tesseract.js/dist/worker.min.js',
        langPath: '../lang-data',
        corePath: '../node_modules/tesseract.js-core/tesseract-core.wasm.js',
        logger: m => (document.getElementById("progress-bar").style.width = m.progress * 100 + "%", $("#progress-bar").html(Math.trunc(m.progress * 100) + "%"))
    });

    (async () => {
        await worker.load();
        await worker.loadLanguage('deu');
        await worker.initialize('deu');
    })();
}

function copyJSON() {
    fetch('/db.json')
        .then((response) => response.json())
        .then((json) => {
            copiedValues = json.thoskaVerif.Matriculation;
            console.log(copiedValues);
        });
}

function capturePhoto() {
    var xhr = new XMLHttpRequest();
    xhr.open('GET', "/capture", true);
    xhr.send();
}

function initiateSteps(){
    const progress = document.getElementById("progress");
    const next = document.getElementById("next");
    const prev = document.getElementById("prev");
    const cricles = document.querySelectorAll(".circle");
  
    next.addEventListener("click", () => {
      if(currentActive != 3){
        
      
      if (currentActive < cricles.length) {
        currentActive++;
      }
      update();
    }
    });
  
    prev.addEventListener("click", () => {
    if (currentActive > 1) {
        currentActive--;
      }
      update();
    });
}

function update() {
    cricles.forEach((circle, idx) => {
      if (idx < currentActive) {
        circle.classList.add("active");
      } else {
        circle.classList.remove("active");
      }
    });

    progress.style.width =
      ((currentActive - 1) / (cricles.length - 1)) * 100 + "%";

    if (currentActive === 1) {
      prev.disabled = true;
    } else if (currentActive === cricles.length) {
      next.disabled = true;
    } else {
      prev.disabled = false;
      next.disabled = false;
    }
}

function forwardButtonClick() {
    $("#next").click(function () {
        var stepClass = setClassForOperation(stepCount);
        if (stepClass == 'stand') {
            fetchResponseFromFaceDetection();
            stepCount++;
        }
        if (stepClass == 'imageConfirm') {
            var matNoFromResponse = getFetchedMatriculationNo() ? getFetchedMatriculationNo() : "";
            matNoFromResponse == "" ? (stepCount--, handleFaceDetectionError()) : stepCount++;
            $('#prev').css('pointer-events', 'none');
        }
        if (stepClass == 'thoskaPlacement') {
            $('#next').css('pointer-events', 'none');
            capturePhoto();
            var img = "http://192.168.0.122/saved-photo/photo.jpg";
            var matNoFromResponse = getFetchedMatriculationNo();
            var matNoFromOCR = performOCR(img);
            verifyResults(matNoFromResponse, matNoFromOCR) ? performEndResult() : handleOCRdetectionError();
            setTimeout(function () {
                reintiateApplication();
            }, 3000);
        }
    });
}

function previousButtonClick() {
    $("#prev").click(function () {
        var stepClass = setClassForOperation(stepCount);
        if (stepClass == 'imageConfirm') {
            stepCount--;
        }
    });
}

function setClassForOperation(stepCount) {
    var className = ['stand', 'imageConfirm', 'thoskaPlacement'];

    return className[stepCount];
}

function fetchResponseFromFaceDetection() {
    fetch("http://192.168.0.122/getVerification")
        .then(response => response.text())
        .then(data => {
            setResponseConstraints(data);
        })
        .catch(error => {
            console.log(error);
        });
}

function setResponseConstraints(res) {
    currentFetchedMat = res.currentFetchedMat;
    var image = res.image;
}

function handleFaceDetectionError() {
    $('#face-recognition-error').fadeIn();
    setTimeout(function () {
        $('#face-recognition-error').fadeOut();
    }, 3000);
}

function handleOCRdetectionError(){
    $('#text-recognition-error').fadeIn();
    setTimeout(function () {
        $('#text-recognition-error').fadeOut();
    }, 3000);
}

function showProgress(){
    $('#progress-bar-container').fadeIn();
}

function getFetchedMatriculationNo() {
    return currentFetchedMat;
}

function performOCR(img) {
    showProgress();
    (async () => {
        const { data: { text } } = await worker.recognize(img);
        var matches = text.match(/\b\d{6}\b/);
        var matNoFromOCR = matches[0] ? matches[0] : handleOCRdetectionError();
        console.log('Matriculation Number From OCR - ' + matNoFromOCR);
        return matNoFromOCR;
    })();
}

function verifyResults(matNoFromResponse, matNoFromOCR){
    if(matNoFromResponse == matNoFromOCR){
        return true;
    }
    else{
        return false;
    }
}

function performEndResult(){
    $('#next').click();
}

function reintiateApplication(){
    stepCount, currentFetchedMat = 0;
    faceDetectionResult = '';
    $('.circle').removeClass("active");
    $('.circle:first').addClass('active');
}
