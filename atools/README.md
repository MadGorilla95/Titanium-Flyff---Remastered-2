<!-- PROJECT LOGO -->
<br />
<p align="center">
    <img src="https://i.imgur.com/1zgrWED.png" alt="Logo" width="80" height="80">
  </a>

  <h3 align="center">Flyff - ATools</h3>
  <p align="center">
    This is a Custom version from the normal ATools.
    <p align="center"><img src=https://i.gyazo.com/108d2564f33701a4c3045758c78acc89.png> </a>
  </p>
  
---
<!-- TABLE OF CONTENTS -->

  <details open="open">
  <summary>Index</summary>
  <ol>
    <li>
      <a href="#Overview">Overview</a>
        <li><a href="#What-my-tools-have-in-addition">What my tools have in addition</a></li>
    </li>
    <li>
        <a href="#How-to-Translate">How to Translate</a>
       <ul>
         <li><a href="#Step-by-Step">Step by Step</a></li> 
       </ul>
       </li> 
    <li>
        <a href="#How-to-Build">How to Build</a>
      <ul>
        <li><a href="#To-compile-you-will-need">To compile you will need</a></li>
        <li><a href="#You-must-also">You must also</a></li>          
      </ul>
    </li>
    <li><a href="#How-to-use-it">How to use it</a></li>
  </ol>
</details>

---

<!-- Overview -->
## <p align="center">Overview</p>


ATools is a little software suite that I developed in order to replace Aeonsoft's tools:
<br >
<br >
[![Modeleditor](https://img.shields.io/badge/-Modeleditor%20=>%20Cola-3279A8?style=for-the-badge&labelColor=black&logo=Monster&logoColor=3279a8)](#)
<br >
[![SfxEditor](https://img.shields.io/badge/-SfxEditor%20=>%20SfxEditor-31a85e?style=for-the-badge&labelColor=black&logo=Stripe&logoColor=31a85e)](#)
<br >
[![GUIEditor](https://img.shields.io/badge/-GUIEditor%20=>%20Daisy-FAF60A?style=for-the-badge&labelColor=black&logo=Groupon&logoColor=FAF60A)](#)
<br >
[![GUIEditor_v19](https://img.shields.io/badge/-GUIEditor_v19-FAF60A?style=for-the-badge&labelColor=black&logo=Groupon&logoColor=FAF60A)](#)
<br >
[![WorldEditor](https://img.shields.io/badge/-WorldEditor%20=>%20Beast-BB2121?style=for-the-badge&labelColor=black&logo=Webflow&logoColor=BB2121)](#)
<br >
[![ResEditor](https://img.shields.io/badge/-ResEditor-17A22E?style=for-the-badge&labelColor=black&logo=Rakuten&logoColor=17A22E)](#)
<br >
<br >
<br >
<br >
#### What my tools have in addition:
* New beautiful and convenient interface
* More stable
* Less bugs
* Better rendering (antialiasing, Z-Buffer with better quality)
* Totally in french, english and german
* Custom keyboards shortcuts
* Lots of new features added
* The source code is available so you can fit all you need

---

<!-- How to Translate -->
## <p align="center">How to Translate</p>

#### What you need when you want to translate something:

QT Linguist  | Worldeditor.pro   |
------------ | -------------     |
[Download Here](https://github.com/thurask/Qt-Linguist/releases/download/20201205/linguist_5.15.2.zip) | Available in Atool folder

#### Step by Step:
1. Open the ``QT 5.14.2 (MinGW) console``
2. Select your hard disk:
    ```CMD
    D:
    ``` 
3. Enter the path of the Worldeditor folder:
    ```CMD
    CD D:\ATools\WorldEditor
    ``` 
4. To update the current .pro file enter the following Code in the console
    ```CMD
    lupdate -pro Worldeditor.pro -ts worldeditor_en.ts
    ``` 
     <a href="https://user-images.githubusercontent.com/51216258/132460691-599df87f-a690-4c07-a961-1da54e441de0.png">Preview CMD</a>

5. We open ``QT Linguist`` and open the file ``worldeditor_en.ts``
6. We can translate everything and when we are done we save it
7. After that we open ``QT 5.14.2 (MinGW)`` again.
8. Now we enter the following
    ```CMD
    lrelease worldeditor_en.ts
    ``` 
    <a href="https://user-images.githubusercontent.com/51216258/132461821-0f80aa96-a199-435f-ae82-e4f7272ae479.png">Preview CMD</a>
    
9. Now we have a file named 
    ```qm
    worldeditor_en.qm  
    ``` 
10. After that we put the file in the languages folder 
     ```qm
    D:\ATools\Resources\plugins\languages\English
    ``` 
    
---

<!-- How to Build -->
## <p align="center">How to Build</p>

#### To compile you will need:

Visual Studio 2019 | DirectX SDK   | Visual Studio Add-in for Qt5 | Qt 5.14.2     |
------------       | ------------- | ------------                 | ------------- | 
[Download Here](https://visualstudio.microsoft.com/en/vs/) | [Download Here](http://www.microsoft.com/en-us/download/details.aspx?id=6812) | [Download Here](https://download.qt.io/official_releases/vsaddin/2.5.1/) | [Download Here](https://www.qt.io/download-qt-installer)


#### You must also:
* Install and configure Qt and his Add-in in order to have this in Visual Studio
![image](https://user-images.githubusercontent.com/51216258/132464937-5fe335a0-449e-4b84-a090-233dc89a8db7.png)
![image](https://user-images.githubusercontent.com/51216258/132465005-4dbef9e9-7ce8-4031-aac2-db1c8996817c.png)


---

<!-- How to use it -->
## <p align="center">How to use it</p>
Copy only your resource file on the folder `Resources`:
* Char
* Client
* Cursor
* Icon
* Item
* LuaFunc
* Model
* Music
* SFX
* Sound
* Theme
* Weather
* World

Copy your file with extension `.txt.txt` on the folder `Translate\ENG\`.<br>
You shall not replace the `.dll` and `.lib` already exist on the folder `Resources`.<br>

---
