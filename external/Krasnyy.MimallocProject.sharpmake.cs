using Sharpmake;

namespace Krasnyy
{
	[Generate]
	public class MimallocProject : BaseProject
	{
		public MimallocProject()
		{
			Name = "Mimalloc";
			SourceRootPath = @"[project.SharpmakeCsPath]/mimalloc/src";

			SourceFilesExcludeRegex.Add("(.*)prim[/\\\\](.*)$");
			SourceFilesExclude.Add(@"[project.SharpmakeCsPath]/mimalloc/src/free.c");
			SourceFilesExclude.Add(@"[project.SharpmakeCsPath]/mimalloc/src/page-queue.c");
			SourceFilesExclude.Add(@"[project.SharpmakeCsPath]/mimalloc/src/alloc-override.c");
			SourceFilesExclude.Add(@"[project.SharpmakeCsPath]/mimalloc/src/static.c");

			SourceFiles.Add(@"[project.SharpmakeCsPath]/mimalloc_prim.c");
			SourceFiles.Remove(Globals.RootDir + "/framework/MemoryOverride.cpp");

			AddTargets(Globals.GetMimallocTargets());
		}

		public override void ConfigureAll(Configuration conf, Target target)
		{
			base.ConfigureAll(conf, target);

			conf.IncludePaths.Add(@"[project.SharpmakeCsPath]/mimalloc/src/prim");

			conf.Output = Configuration.OutputType.Lib;
		}
	}
}

