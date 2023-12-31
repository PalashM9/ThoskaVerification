**Step 1: Download and Build the Latest Library using Software Network Client**

Download the latest SW (Software Network) client from https://software-network.org/client/sw-master-windows-client.zip
Extract the downloaded file and open a DOS command prompt

Run the following commands in the command prompt:

```
sw setup
sw build org.sw.demo.google.tesseract.tesseract-master
```

![image](https://user-images.githubusercontent.com/100582448/227931718-2591fb4a-4c57-4221-b65e-a49a8222365b.png)

**Step 2: Install Git**

Download Git for Windows from https://git-scm.com/download/win
Install Git by running the executable file
In the example provided, the 64-bit version was used

**Step 3: Set Up Vcpkg (Visual C++ Package Manager) for Tesseract in Visual Studio**

Clone the Vcpkg package from https://github.com/microsoft/vcpkg using Git command in the command prompt

Run the vcpkg bootstrap script by executing the following command in the command prompt: 

```
.\vcpkg\bootstrap-vcpkg.bat
```
Install the Tesseract libraries needed for the project by executing the following command in the command prompt: 

```
.\vcpkg\vcpkg install tesseract:x64-windows
```

![image](https://user-images.githubusercontent.com/100582448/227931794-32b7716d-fb89-4c1c-ae05-47c4e9177e47.png)

![image](https://user-images.githubusercontent.com/100582448/227931871-dc678d77-be50-49ee-916d-d6a9a318bbb7.png)

**Step 4: Integrate Vcpkg with Visual Studio**

To use Vcpkg with Visual Studio, run the following command in the command prompt (may require administrator elevation): 

```
.\vcpkg\vcpkg integrate install
```

**Step 5: Involves downloading the tesseract language data**

It can be found on the tessdata download link provided on the GitHub page.
The "git clone" command is used to download the "eng" files to a specific folder location. 
The Visual Studio solution folder needs to have the include and data folders set up, with the "tessdata" folder containing the tesseract data files
mentioned in Step 5 and the "tesseract-include" folder containing the lib/dll files necessary for the project. 
The tesseract/leptonica .lib library files can be obtained from the "packages" folder installed earlier, 
while the tesseract/leptonica dll and other files can be obtained from the "tools" folder installed earlier. 
The header files can be obtained from the "include" folder that was installed earlier,
and they should be copied into the "tesseract-include{tesseract, leptonica}" folders that were created for the Visual Studio project.

![image](https://user-images.githubusercontent.com/100582448/227931948-ad9a13cc-54cb-45ac-933b-024326c84415.png)
![image](https://user-images.githubusercontent.com/100582448/227932035-15746bc8-e2b6-4f4f-887e-96ffabd7e188.png)
![image](https://user-images.githubusercontent.com/100582448/227932056-6819a4f3-8b09-4bb2-951c-5ab58719a598.png)
![image](https://user-images.githubusercontent.com/100582448/227932076-5caade09-e07e-4211-9c9b-fcb466e7daa5.png)


**Step 6 : Set up the project properties**

After copying over the necessary files for the sample project. 
This can be done by right-clicking on the project folder in Visual Studio and selecting properties. 
Then, under General > VC++ Directories, it should set the include directories.

![image](https://user-images.githubusercontent.com/100582448/227932131-98708621-7d07-4c3f-be8f-ca9dbf40309c.png)
