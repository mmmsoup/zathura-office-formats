# zathura-office-formats

a zathura plugin inspired by [zaread](https://github.com/paoloap/zaread) to view .docx and other file formats in zathura. Uses libreoffice to convert files to pdfs and then poppler to render them (shamelessly reusing pretty much all of the code in [zathura-pdf-poppler](https://github.com/pwmt/zathura-pdf-poppler)). Converted files are cached in `~/.cache/zathura-office-formats` as it takes a little while to convert the files to pdfs.

## dependencies
- libreoffice
- poppler
- zathura

## install
```
git clone --recurse-submodules https://github.com/mmmsoup/zathura-office-formats
cd zathura-office-formats
make
sudo make install
```

## usage
simply run `zathura path/to/file.docx` or `:open path/to/file.docx` from within zathura

## uninstall
either run `sudo make uninstall` or manually remove `/usr/lib/zathura/libzathura-office-formats.so`.
