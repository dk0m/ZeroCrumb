
# ZeroCrumb

Bypassing Chrome Elevation Service (Without UAC) To Dump App Bound Keys & Decrypt Cookies.

## Explanation

ZeroCrumb bypasses the Chrome Elevation Service by impersonating a Chrome instance using [Transacted Hollowing](https://github.com/hasherezade/transacted_hollowing/), Effectively allowing us to use the IElevator COM interface to decrypt the App Bound Key.

Then, The key dumper running the in the hollowed chrome instance will send the decrypted key back to ZeroCrumb via a [Named Pipe](https://learn.microsoft.com/en-us/windows/win32/ipc/named-pipes).

After this, The user can decrypt cookies with the key we fetched.

## Merits With The Named Pipe Approach
Usage of named pipes in ZeroCrumb allows any program, As long as it can use the Windows API, To connect to the ZeroCrumb named pipe and read the App Bound Key from it. 

## Using ZeroCrumb As A Library
If you want to use ZeroCrumb as a library, You will need to compile a DLL that exports a function that does the hollowing of chrome with the key dumper. You might also need to embed the key dumper PE in the .rsrc section of the compiled DLL and later fetch it with the application resources Windows APIs.

## Usage
You can run this in any directory of your choice as long as the key dumper is in the same directory.
```
ZeroCrumb.exe <BROWSER_TYPE>
```
```
Browser Types:
0 -> Chrome
1 -> Brave
2 -> Edge
```

## Example
```
$ ZeroCrumb.exe 0
[*] Reading From Pipe...
App Bound Key: 980f8ea8af3299d966a26242.....
```

## Disclaimer
This is by no means the most stealthy implementation of this bypass, It can get detected by checking for specific strings, hooking APIs, etc.
By the time I'm testing this, It got past Windows Defender but I haven't checked other AVs.
I'll leave making the bypass stealthier as an exercise for the user if this bypass gets signatured in the future.

## Credits
[Transacted Hollowing](https://github.com/hasherezade/transacted_hollowing/) by [hasherezade](https://github.com/hasherezade/).

[App Bound Encryption Decryption](https://github.com/xaitax/Chrome-App-Bound-Encryption-Decryption/) by [Xaitax](https://github.com/xaitax/).
