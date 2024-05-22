using Sharpmake;

namespace Krasnyy
{
	[Generate]
	public class MainSolution : Solution
	{
		public MainSolution()
		{
			Name = "Krasnyy";

			AddTargets(Globals.GetTargets());
		}

		[Configure]
		public void ConfigureAll(Solution.Configuration conf, Target target)
		{
			conf.SolutionPath = Globals.RootDirectory + @"/projects/[target.DevEnv]";
			conf.SolutionFileName = @"[solution.Name]";

			conf.AddProject<FrameworkProject>(target);
			conf.AddProject<LauncherProject>(target);
			conf.SetStartupProject<LauncherProject>();
		}
	}
}
