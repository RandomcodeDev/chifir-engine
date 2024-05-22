using Sharpmake;

namespace Krasnyy
{
	public class BaseProject : Project
	{
		public BaseProject()
		{
			SourceRootPath = @"[project.SharpmakeCsPath]";
		}

		[Configure]
		public void ConfigureAll(Project.Configuration conf, Target target)
		{
			conf.ProjectPath = Globals.RootDirectory + @"/projects/[target.DevEnv]";
			conf.ProjectName = @"[project.Name]";

			conf.IncludePaths.Add(Globals.RootDirectory);
			conf.IncludePaths.Add(@"[project.SourceRootPath]");

			switch (target.Platform)
			{
			case Platform.win32:
				conf.Defines.Add("KR_PLATFORM_WINDOWS");
				conf.Defines.Add("KR_ARCHITECTURE_X86");
				break;
			case Platform.win64:
				conf.Defines.Add("KR_PLATFORM_WINDOWS");
				conf.Defines.Add("KR_ARCHITECTURE_X86");
				conf.Defines.Add("KR_ARCHITECTURE_X64");
				break;
			case Platform.linux:
				conf.Defines.Add("KR_PLATFORM_LINUX");
				conf.Defines.Add("KR_ARCHITECTURE_X86");
				conf.Defines.Add("KR_ARCHITECTURE_X64");
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
