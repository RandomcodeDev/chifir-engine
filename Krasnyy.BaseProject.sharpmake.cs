using Sharpmake;

namespace Krasnyy
{
	public class BaseProject : Project
	{
		public BaseProject()
		{
			SourceRootPath = @"[project.SharpmakeCsPath]";

			SourceFiles.Add(Globals.RootDir + "/framework/MemoryOverride.cpp");
		}

		[Configure]
		public virtual void ConfigureAll(Configuration conf, Target target)
		{
			conf.ProjectPath = Globals.RootDir + @"/projects/[target.DevEnv]";
			conf.ProjectName = @"[project.Name]";

			conf.IntermediatePath = Globals.ObjectDir + @"/[project.Name]/[target.Optimization]/[target.Platform]";
			conf.TargetPath = Globals.OutputDir + @"/[target.Optimization]/[target.Platform]";

			conf.IncludePaths.Add(Globals.RootDir);
			conf.IncludePaths.Add(@"[project.SourceRootPath]");
			conf.IncludePaths.Add(Globals.RootDir + "/external/mimalloc/include");

			switch (target.Platform)
			{
			case Platform.win32:
				conf.Defines.Add("KR_PLATFORM_WINDOWS");
				conf.Defines.Add("KR_ARCHITECTURE_X86");
				conf.Defines.Add("KR_USE_MIMALLOC");
				break;
			case Platform.win64:
				conf.Defines.Add("KR_PLATFORM_WINDOWS");
				conf.Defines.Add("KR_ARCHITECTURE_X86");
				conf.Defines.Add("KR_ARCHITECTURE_X64");
				conf.Defines.Add("KR_USE_MIMALLOC");
				break;
			case Platform.linux:
				conf.Defines.Add("KR_PLATFORM_LINUX");
				conf.Defines.Add("KR_ARCHITECTURE_X86");
				conf.Defines.Add("KR_ARCHITECTURE_X64");
				conf.Defines.Add("KR_USE_MIMALLOC");
				break;
			case Platform.nx:
				conf.Defines.Add("KR_PLATFORM_SWITCH");
				conf.Defines.Add("KR_ARCHITECTURE_ARM64");
				break;
			}

			switch (target.Optimization)
			{
			case Optimization.Debug:
				conf.Defines.Add("KR_DEBUG");
				break;
			case Optimization.Release:
				conf.Defines.Add("KR_RELEASE");
				break;
			case Optimization.Retail:
				conf.Defines.Add("KR_RETAIL");
				break;
			}
		}
	}
}
