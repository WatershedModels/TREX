# TREX: <u>T</u>wo-Dimensional <u>R</u>unoff <u>E</u>rosion and E<u>x</u>port
Revised TREX watershed model, source code from [CSU](http://www.engr.colostate.edu/~pierre/ce_old/Projects/TREX%20Web%20Pages/TREX-Home.html).

# 3. Update with the latest TREX source (For administrator only)

In order to make it easier to track code changes and keep up with newer revisions, I use subtree to manage the [TREX source code from CSU](https://github.com/crazyzlj/trex_src_csu).

+ Set subtree as `trex_src`(only do once at the very beginning of this repository)

  ```bash
   git remote add -f trex_src git@github.com:crazyzlj/trex_src_csu.git
   git subtree add --prefix=trex_src trex_src master --squash
  ```

+ Do any modification to the `trex_src` to meet our specific needs.

+ If the TREX source is updated we should pull the latest code from [trex_src_csu](https://github.com/crazyzlj/trex_src_csu) and merge it.

  ```shell
  git fetch trex_src master
  git subtree pull --prefix=trex_src trex_src master --squash
  ```

# 4. Changelog

+ 10/12/2017: initial by the source code updated in 2/10/2011.