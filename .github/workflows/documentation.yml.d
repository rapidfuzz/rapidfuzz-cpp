name: documentation

on:
  push:
    branches:
      - main

jobs:
  build_docs:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v2
    - run: sudo apt-get install -y doxygen
    - run: doxygen ./Doxyfile
    - uses: peaceiris/actions-gh-pages@v3
      with:
        github_token: ${{ secrets.GITHUB_TOKEN }}
        publish_dir: ./doxygen/html
