def options(opt):
    opt.load('compiler_cxx')


def configure(conf):
    conf.load('compiler_cxx')

    conf.start_msg('Checking for gtest')

    conf.check_linux()

    conf.env.INCLUDES_LIBGTEST = [
        '/usr/include',
        '/usr/include/gtest'
    ]

    conf.env.LIBPATH_LIBGTEST = [
        '/usr/lib',
        '/usr/lib64'
    ]

    LIBS = [
        ['gtest'],
        ['gtest_main']
    ]

    for libs in LIBS:
        conf.env.LIB_LIBGTEST = libs
        try:
            conf.run_c_code(
                code='''
                    #include <gtest/gtest.h>

                    int zero() {
                        return 0;
                    }

                    TEST(zero, zero) {
                        EXPECT_EQ(0, zero());
                    }

                    int main(int argc, char **argv) {
                        testing::InitGoogleTest(&argc, argv);
                        return RUN_ALL_TESTS();
                    }
                ''',
                use='LIBGTEST',
                compile_filename='libgtest.cpp',
                env=conf.env.derive(),
                define_ret=True,
                features=['cxx', 'cxxprogram', 'test_exec']
            )
        except:
            conf.end_msg('not found')
            conf.fatal('Library gtest not found')
        conf.end_msg('found')
        return
