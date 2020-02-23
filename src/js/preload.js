const imageProcess = require('../build/Release/module');
const fs = require('fs');

// All of the Node.js APIs are available in the preload process.
// It has the same sandbox as a Chrome extension.
window.addEventListener('DOMContentLoaded', () => {
  let tempDir = `${__dirname}/../temp`;
  function clearTemp() {
    return new Promise((resolve) => {
      fs.readdir(tempDir, (err, files) => {
        if (err) throw err;

        for (const file of files) {
          fs.unlinkSync(`${tempDir}/${file}`, (err) => {
            if (err) throw err;
          });
        }
        resolve();
      });
    });
  }
  function resetInputs() {
    $('input[type=range]').val(100);
  }
  clearTemp();
  resetInputs();

  let imageBefore = $('#image-before');
  let imageAfter = $('#image-after');
  let currentFile = '';
  let currentFileModified = '';
  let currentFileOrigin = '';

  async function updateImg() {
    if (fs.existsSync(`${tempDir}/${currentFileModified}`)) {
      imageBefore.attr(
        'src',
        `${tempDir}/${currentFile}?${new Date().getTime()}`,
      );
      if (currentFileModified != '') {
        imageAfter.attr(
          'src',
          `${tempDir}/${currentFileModified}?${new Date().getTime()}`,
        );
      }
      else imageAfter.attr('src', 'placeholder.png');
    }
    else {
      setTimeout(updateImg, 10);
    }
  }

  $('#select-img').on('click', async () => {
    $('#file-select').click();
  });
  $('#save-edit').on('click', () => {
    if (currentFileModified == '') return;
    fs.copyFileSync(
      `${tempDir}/${currentFileModified}`,
      `${tempDir}/${currentFile}`,
    );
    updateImg();
    resetInputs();
  });
  $('#save-img').on('click', () => {
    if (currentFileModified == '') return;
    let finalPath = currentFileOrigin.replace(currentFile, '');
    fs.copyFileSync(
      `${tempDir}/${currentFileModified}`,
      `${finalPath}${currentFileModified}`,
    );
  });
  $('#file-select').change(() => {
    imageBefore.attr('src', 'placeholder.png');
    imageAfter.attr('src', 'placeholder.png');
    clearTemp().then(() => {
      let file = $('#file-select').prop('files')[0];
      fs.copyFileSync(file.path, `${tempDir}/${file.name}`);
      imageBefore.attr('src', `${tempDir}/${file.name}`);
      currentFileOrigin = file.path;
      currentFile = file.name;
      $('#file-select').val('');
    });

    $('.disabled').removeClass('disabled');
    $('#select-img').blur();
  });

  $('#saturation').on('input', () => {
    if (currentFile == '') return;
    let relativeChange = $('#saturation').val() - 100;

    if (
      currentFileModified != '' &&
      fs.existsSync(`${tempDir}/${currentFileModified}`)
    ) {
      fs.unlinkSync(`${tempDir}/${currentFileModified}`);
    }
    currentFileModified = imageProcess.saturation(
      currentFile,
      tempDir,
      relativeChange,
    );
    updateImg();
  });
  $('#brightness').on('input', () => {
    if (currentFile == '') return;
    let relativeChange = $('#brightness').val() - 100;

    if (
      currentFileModified != '' &&
      fs.existsSync(`${tempDir}/${currentFileModified}`)
    ) {
      fs.unlinkSync(`${tempDir}/${currentFileModified}`);
    }
    currentFileModified = imageProcess.brightness(
      currentFile,
      tempDir,
      relativeChange,
    );
    updateImg();
  });
  $('#warm-btn').on('click', () => {
    currentFileModified = imageProcess.warm(currentFile, tempDir);
    updateImg();
  });
  $('#cool-btn').on('click', () => {
    currentFileModified = imageProcess.cool(currentFile, tempDir);
    updateImg();
  });
  $('#grey-btn').on('click', () => {
    currentFileModified = imageProcess.grey(currentFile, tempDir);
    updateImg();
  });
  $('#creama-btn').on('click', () => {
    currentFileModified = imageProcess.creama(currentFile, tempDir);
    updateImg();
  });
  $('#lark-btn').on('click', () => {
    currentFileModified = imageProcess.lark(currentFile, tempDir);
    updateImg();
  });
  $('#rise-btn').on('click', () => {
    currentFileModified = imageProcess.rise(currentFile, tempDir);
    updateImg();
  });
});
