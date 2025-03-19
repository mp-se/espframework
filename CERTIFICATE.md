# How to create a self signed certificate that is accepted

1. Download and install openssl

2. Run the following commands

```bash
# Generate the device key
openssl genpkey -out server.key -algorithm RSA -pkeyopt rsa_keygen_bits:2048

# Create a signing request
openssl req -new -key server.key -out server.csr

# Sign the request
openssl x509 -req -days 365 -in server.csr -signkey server.key -out server.crt

# Optional, validate the certificate
openssl x509 -in server.crt -noout -issuer -subject
```

3. Upload the `server.key` and `server.crt` to the device and restart it (Option under the tools section). It should the start the webserver in SSL mode.

## MacOS

4. Open `KeyChain Access` application and use `File -> Import Items` to import the `server.crt`file. 
5. Find the certificate and set trust to `Always Trust` for the `SSL` option.
6. The certificate should now be trused by your browser.

## Windows

4. Open `certmgr.msc` from the commandline
5. Navigate to `Trusted Root Certificate Authorities -> Certificates` and use `Import` to import the `server.crt`file Follow the wizard.
6. The certificate should now be trused by your browser.
