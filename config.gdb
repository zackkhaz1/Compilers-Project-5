set breakpoint pending on
set confirm off
file ./cronac
break crona::Err::report
commands
	where
end
break crona::InternalError::InternalError
commands
	where
end
break crona::ToDoError::ToDoError
commands
	where
end

define p5
  set args p5_tests/$arg0.cronac -n --
  run
end
